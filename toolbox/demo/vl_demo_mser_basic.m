% VL_DEMO_MSER_BASIC  Demo: MSER: basic functionality

pfx = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------
I = imread(fullfile(vl_root,'data','spots.jpg')) ;

image(I) ; colormap gray ;
axis equal ; axis off ; axis tight ;
vl_demo_print('mser_basic_0') ;

% --------------------------------------------------------------------
%                                       Convert the to required format
% --------------------------------------------------------------------
I = uint8(rgb2gray(I)) ;

clf ; imagesc(I) ; colormap(gray(256)) ;
axis equal ; axis off ; axis tight ;

% --------------------------------------------------------------------
%                                                             Run VL_MSER
% --------------------------------------------------------------------
[r,f] = vl_mser(I,'MinDiversity',0.7,'MaxVariation',0.2,'Delta',10) ;

% compute regions mask
M = zeros(size(I)) ;
for x=r'
  s = vl_erfill(I,x) ;
  M(s) = M(s) + 1;
end

% adjust convention
f = vl_ertr(f) ;

figure(1) ;
hold on ;
h1 = vl_plotframe(f) ; set(h1,'color','y','linewidth',3) ;
h2 = vl_plotframe(f) ; set(h2,'color','k','linewidth',1) ;
vl_demo_print('mser_basic_frames') ;

figure(2) ; clf ; imagesc(I) ; hold on ;
colormap(gray(256)) ;
if vl_isoctave()
  [c,h]=contour(M,(0:max(M(:)))+.5,'y','linewidth',3) ;
else
  [c,h]=contour(M,(0:max(M(:)))+.5) ;
  set(h,'color','y','linewidth',3) ;
end
axis equal ; axis off ;
vl_demo_print('mser_basic_contours') ;

% --------------------------------------------------------------------
%                                                       Run VL_MSER again
% --------------------------------------------------------------------

[r,f] = vl_mser(I,'MinDiversity',0.7,'MaxVariation',0.2,...
  'Delta',10,'BrightOnDark',1,'DarkOnBright',0) ;

% adjust convention
f = vl_ertr(f) ;

% compute regions mask
M = zeros(size(I)) ;
for x=r'
  s = vl_erfill(I,x) ;
  M(s) = M(s) + 1;
end

figure(1) ;
hold on ;
h1 = vl_plotframe(f) ; set(h1,'color','g','linewidth',3) ;
h2 = vl_plotframe(f) ; set(h2,'color','k','linewidth',1) ;
vl_demo_print('mser_basic_frames_both') ;

figure(2) ;
if vl_isoctave()
  [c,h]=contour(M,(0:max(M(:)))+.5,'y','linewidth',3) ;
else
  [c,h]=contour(M,(0:max(M(:)))+.5) ;
  set(h,'color','g','linewidth',3) ;
end

hold on ; axis equal ; axis off ;
vl_demo_print('mser_basic_contours_both') ;
