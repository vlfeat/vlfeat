% DEMO_MSER_BASIC  Demo: MSER: basic functionality

pfx = fullfile(vlfeat_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------
I = imread(fullfile(vlfeat_root,'data','spots.jpg')) ;

image(I) ; colormap gray ;
axis equal ; axis off ; axis tight ;
demo_print('mser_basic_0') ;

% --------------------------------------------------------------------
%                                       Convert the to required format
% --------------------------------------------------------------------
I = uint8(rgb2gray(I)) ;

clf ; imagesc(I) 
axis equal ; axis off ; axis tight ;
demo_print('mser_basic_1') ;

% --------------------------------------------------------------------
%                                                             Run MSER
% --------------------------------------------------------------------
[r,f] = mser(I,'MinDiversity',0.7,'MaxVariation',0.2,'Delta',10) ;

% compute regions mask
M = zeros(size(I)) ;
for x=r'
  s = erfill(I,x) ;
  M(s) = M(s) + 1;
end

% adjust convention
f = ertr(f) ;

figure(1) ;
hold on ;
h1 = plotframe(f) ; set(h1,'color','y','linewidth',3) ;
h2 = plotframe(f) ; set(h2,'color','k','linewidth',1) ;

figure(2) ; clf ; imagesc(I) ; hold on ; colormap gray ;
[c,h]=contour(M,(0:max(M(:)))+.5) ;
set(h,'color','y','linewidth',3) ;
axis equal ; axis off ;

% --------------------------------------------------------------------
%                                                       Run MSER again
% --------------------------------------------------------------------

[r,f] = mser(uint8(255-I),'MinDiversity',0.7,'MaxVariation',0.2,'Delta',10) ;

% adjust convention
f = ertr(f) ;

% compute regions mask
M = zeros(size(I)) ;
for x=r'
  s = erfill(uint8(255-I),x) ;
  M(s) = M(s) + 1;
end

figure(1) ;
hold on ;
h1 = plotframe(f) ; set(h1,'color','g','linewidth',3) ;
h2 = plotframe(f) ; set(h2,'color','k','linewidth',1) ;

demo_print('mser_basic_2') ;

figure(2) ;
[c,h]=contour(M,(0:max(M(:)))+.5) ;
set(h,'color','g','linewidth',3) ;

hold on ; axis equal ; axis off ;
demo_print('mser_basic_1') ;
