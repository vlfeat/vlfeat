% VL_DEMO_COVDET Demo: VL_COVDET()

% --------------------------------------------------------------------
%                                                               Basics
% --------------------------------------------------------------------

im = vl_impattern('roofs1') ;
im = im(end-255:end,1:320,:) ;

figure(1) ; clf ;
image(im) ; axis image off ;
vl_demo_print('covdet_basic_image') ;

imgs = im2single(rgb2gray(im)) ;
frames = vl_covdet(imgs, 'verbose') ;

hold on ;
vl_plotframe(frames) ;
vl_demo_print('covdet_basic_frames') ;

% --------------------------------------------------------------------
%                                                    Affine adaptation
% --------------------------------------------------------------------

frames = vl_covdet(imgs, 'estimateAffineShape', true, 'verbose') ;

figure(2) ; clf ;
image(im) ; axis image off ; hold on ;
vl_plotframe(frames) ;
vl_demo_print('covdet_basic_affine_frames') ;

% --------------------------------------------------------------------
%                                              Estimating orientations
% --------------------------------------------------------------------

frames = vl_covdet(imgs, 'estimateOrientation', true, 'verbose') ;

figure(3) ; clf ;
image(im) ; axis image off ; hold on ;
vl_plotframe(frames) ;
vl_demo_print('covdet_basic_oriented_frames') ;

% --------------------------------------------------------------------
%                                                   Extracting patches
% --------------------------------------------------------------------

[frames, patches] = vl_covdet(imgs, 'descriptor', 'patch') ;

figure(4) ; clf ;
w = sqrt(size(patches,1)) ;
vl_imarraysc(reshape(patches(:,1:10*10), w,w,[])) ;
axis image off ; hold on ; colormap gray ;
vl_demo_print('covdet_basic_oriented_patches') ;


[frames, patches] = vl_covdet(imgs, ...
                              'descriptor', 'patch' ,...
                              'estimateAffineShape', true, ...
                              'estimateOrientation', false) ;

figure(5) ; clf ;
w = sqrt(size(patches,1)) ;
vl_imarraysc(reshape(patches(:,1:10*10), w,w,[])) ;
axis image off ; hold on ; colormap gray ;
vl_demo_print('covdet_basic_oriented_affine_patches') ;

% --------------------------------------------------------------------
%                                                  Different detectors
% --------------------------------------------------------------------

names = {'DoG', 'Hessian', ...
         'HarrisLaplace', 'HessianLaplace', ...
         'MultiscaleHarris', 'MultiscaleHessian'} ;
figure(6) ; clf ;
for i = 1:numel(names)
  frames = vl_covdet(imgs, 'method', names{i}) ;

  vl_tightsubplot(3,2,i, 'margintop',0.025) ;
  %subplot(3,2,i) ;
  imagesc(im) ; axis image off ;
  hold on ;
  vl_plotframe(frames) ;
  title(names{i}) ;
end

vl_figaspect(2/3) ;
vl_demo_print('covdet_basic_oriented_affine_patches',1) ;
