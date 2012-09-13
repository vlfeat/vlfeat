% VL_DEMO_HOG  Demo: HOG features

% Author: Andrea Vedaldi

% AUTORIGHTS

prefix = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------

im = imread(fullfile(vl_root,'data','roofs1.jpg')) ;
im = im2single(im) ;
im = im(1:128,end-128+1:end,:) ;

figure(1) ; clf ;
image(im) ;
axis equal off tight ;
vl_demo_print('hog_image') ;

% --------------------------------------------------------------------
%                                                 Compute HOG features
% --------------------------------------------------------------------

% Standard features
cellSize = 8 ;
hog = vl_hog(im, cellSize, 'verbose') ;
imhog = vl_hog('render', hog, 'verbose') ;

figure(2) ; clf ;
imagesc(imhog) ;
axis image off ; colormap gray ;
vl_demo_print('hog_features') ;

% Dalal-Triggs variant
cellSize = 8 ;
hog = vl_hog(im, cellSize, 'verbose', 'variant', 'dalaltriggs') ;
imhog = vl_hog('render', hog, 'verbose', 'variant', 'dalaltriggs') ;

figure(3) ; clf ;
imagesc(imhog) ;
axis image off ; colormap gray ;
vl_demo_print('hog_features_dalal_triggs') ;

% --------------------------------------------------------------------
%                                                    Flip HOG features
% --------------------------------------------------------------------

hog = vl_hog(im, cellSize) ;
hogFromFlippedImage = vl_hog(im(:,end:-1:1,:), cellSize) ;
perm = vl_hog('permutation') ;
flippedHog = hog(:,end:-1:1,perm) ;

imHog = vl_hog('render', hog) ;
imHogFromFlippedImage = vl_hog('render', hogFromFlippedImage) ;
imFlippedHog = vl_hog('render', flippedHog) ;

figure(4) ; clf ;
subplot(1,3,1) ; imagesc(imHog) ;
axis image off ; title('HOG features') ;
subplot(1,3,2) ; imagesc(imHogFromFlippedImage) ;
axis image off ; title('Flipping the image') ;
subplot(1,3,3) ; imagesc(imFlippedHog) ;
axis image off ; title('Flipping the features') ;
colormap gray ;
vl_demo_print('hog_flipping',1) ;

% --------------------------------------------------------------------
%                                                 Other HOG parameters
% --------------------------------------------------------------------

figure(5) ; clf ;
numOrientationsRange = [3, 4, 5, 9, 21] ;
for i = 1:numel(numOrientationsRange)
  vl_tightsubplot(1, numel(numOrientationsRange), i) ;
  o = numOrientationsRange(i) ;
  hog = vl_hog(im, cellSize, 'verbose', 'numOrientations', o) ;
  imhog = vl_hog('render', hog, 'verbose', 'numOrientations', o) ;
  imagesc(imhog) ;
  axis image off ;
  colormap gray ;
end
vl_demo_print('hog_num_orientations',.9) ;

figure(6) ; clf ;
[x,y] = meshgrid(linspace(-1,1,128)) ;
im = single(sqrt(x.^2+y.^2)) ;
hog1 = vl_hog(im,cellSize,'numOrientations', 4) ;
hog2 = vl_hog(im,cellSize,'numOrientations', 4,'bilinearOrientations') ;
imhog1 = vl_hog('render',hog1,'numOrientations', 4) ;
imhog2 = vl_hog('render',hog2,'numOrientations', 4) ;

subplot(1,3,1) ; imagesc(im) ;
axis image off ; title('Input image') ;
subplot(1,3,2) ; imagesc(imhog1) ;
axis image off ; title('Hard orientation assignments') ;
subplot(1,3,3) ; imagesc(imhog2) ;
axis image off ; title('Soft orientation assignments') ;
colormap gray ;
vl_demo_print('hog_bilinear_orientations',1) ;
