% DEMO_SIFT_BASIC  Demo: SIFT: basic functionality
pfx = fullfile(vlfeat_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------
I = imread(fullfile(vlfeat_root,'data','a.jpg')) ;

image(I) ; colormap gray ;
axis equal ; axis off ; axis tight ;
demo_print('sift_basic_0') ;

% --------------------------------------------------------------------
%                                       Convert the to required format
% --------------------------------------------------------------------
I = single(rgb2gray(I)) ;

clf ; imagesc(I) 
axis equal ; axis off ; axis tight ;
demo_print('sift_basic_1') ;

% --------------------------------------------------------------------
%                                                             Run SIFT
% --------------------------------------------------------------------
[f,d] = sift(I) ;

hold on ;
perm = randperm(size(f,2)) ; 
sel  = perm(1:50) ;
h1   = plotframe(f(:,sel)) ; set(h1,'color','y','linewidth',3) ;
h2   = plotframe(f(:,sel)) ; set(h2,'color','k','linewidth',1) ;

demo_print('sift_basic_2') ;

h3 = plotsiftdescriptor(d(:,sel),f(:,sel)) ;  set(h3,'color','g') ;

demo_print('sift_basic_3') ;

% --------------------------------------------------------------------
%                                                      Custom keypoint
% --------------------------------------------------------------------
fc = [100;100;10;pi/8] ;
[f,d] = sift(I,'frames',fc) ;

delete([h1 h2 h3]) ;

h1   = plotframe(f) ; set(h1,'color','y','linewidth',3) ;
h2   = plotframe(f) ; set(h2,'color','k','linewidth',1) ;
h3   = plotsiftdescriptor(d,f) ;  set(h3,'color','g') ;

demo_print('sift_basic_4') ;

% --------------------------------------------------------------------
%                                   Custom keypoints with orientations
% --------------------------------------------------------------------

fc = [100;100;10;pi/8] ;
[f,d] = sift(I,'frames',fc,'orientations') ;

delete([h1 h2 h3]) ;

h1   = plotframe(f) ; set(h1,'color','y','linewidth',3) ;
h2   = plotframe(f) ; set(h2,'color','k','linewidth',1) ;
h3   = plotsiftdescriptor(d,f) ;  set(h3,'color','g') ;

demo_print('sift_basic_5') ;

