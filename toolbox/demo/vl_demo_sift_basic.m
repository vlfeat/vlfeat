% VL_DEMO_SIFT_BASIC  Demo: SIFT: basic functionality

pfx = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------
I = imread(fullfile(vl_root,'data','a.jpg')) ;

image(I) ; colormap gray ;
axis equal ; axis off ; axis tight ;
vl_demo_print('sift_basic_0') ;

% --------------------------------------------------------------------
%                                       Convert the to required format
% --------------------------------------------------------------------
I = single(rgb2gray(I)) ;

clf ; imagesc(I) 
axis equal ; axis off ; axis tight ;
vl_demo_print('sift_basic_1') ;

% --------------------------------------------------------------------
%                                                             Run SIFT
% --------------------------------------------------------------------
[f,d] = vl_sift(I) ;

hold on ;
perm = randperm(size(f,2)) ; 
sel  = perm(1:50) ;
h1   = vl_plotframe(f(:,sel)) ; set(h1,'color','y','linewidth',3) ;
h2   = vl_plotframe(f(:,sel)) ; set(h2,'color','k','linewidth',1) ;

vl_demo_print('sift_basic_2') ;

h3 = vl_plotsiftdescriptor(d(:,sel),f(:,sel)) ;  set(h3,'color','g') ;

vl_demo_print('sift_basic_3') ;

% --------------------------------------------------------------------
%                                                      Custom keypoint
% --------------------------------------------------------------------
fc = [100;100;10;pi/8] ;
[f,d] = vl_sift(I,'frames',fc) ;

delete([h1 h2 h3]) ;

h1   = vl_plotframe(f) ; set(h1,'color','y','linewidth',3) ;
h2   = vl_plotframe(f) ; set(h2,'color','k','linewidth',1) ;
h3   = vl_plotsiftdescriptor(d,f) ;  set(h3,'color','g') ;

vl_demo_print('sift_basic_4') ;

% --------------------------------------------------------------------
%                                   Custom keypoints with orientations
% --------------------------------------------------------------------

fc = [100;100;10;pi/8] ;
[f,d] = vl_sift(I,'frames',fc,'orientations') ;

delete([h1 h2 h3]) ;

h1   = vl_plotframe(f) ; set(h1,'color','y','linewidth',3) ;
h2   = vl_plotframe(f) ; set(h2,'color','k','linewidth',1) ;
h3   = vl_plotsiftdescriptor(d,f) ;  set(h3,'color','g') ;

vl_demo_print('sift_basic_5') ;
