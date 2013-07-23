% VL_DEMO_COVDET Demo: VL_COVDET()

% --------------------------------------------------------------------
%                                                               Basics
% --------------------------------------------------------------------

im = vl_impattern('roofs1') ;
im = im(end-128:end,128:320,:) ;

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
vl_demo_print('covdet_affine_frames',.8) ;

% --------------------------------------------------------------------
%                                              Estimating orientations
% --------------------------------------------------------------------

frames = vl_covdet(imgs, 'estimateOrientation', true, 'verbose') ;

figure(3) ; clf ;
image(im) ; axis image off ; hold on ;
vl_plotframe(frames) ;
vl_demo_print('covdet_oriented_frames',.8) ;

% --------------------------------------------------------------------
%                                                   Extracting patches
% --------------------------------------------------------------------

[frames, patches] = vl_covdet(imgs, 'descriptor', 'patch') ;

figure(4) ; clf ;
w = sqrt(size(patches,1)) ;
vl_imarraysc(reshape(patches(:,1:10*10), w,w,[])) ;
axis image off ; hold on ; colormap gray ;
vl_demo_print('covdet_patches') ;

[frames, patches] = vl_covdet(imgs, ...
                              'descriptor', 'patch' ,...
                              'estimateAffineShape', true, ...
                              'estimateOrientation', false) ;

figure(5) ; clf ;
w = sqrt(size(patches,1)) ;
vl_imarraysc(reshape(patches(:,1:10*10), w,w,[])) ;
axis image off ; hold on ; colormap gray ;
vl_demo_print('covdet_affine_patches') ;

% --------------------------------------------------------------------
%                                                  Different detectors
% --------------------------------------------------------------------

names = {'DoG', 'Hessian', ...
         'HarrisLaplace', 'HessianLaplace', ...
         'MultiscaleHarris', 'MultiscaleHessian'} ;
figure(6) ; clf ;
for i = 1:numel(names)
  frames = vl_covdet(imgs, 'method', names{i}) ;

  vl_tightsubplot(3,2,i, 'margintop',0.025, 'marginright', 0.01) ;
  imagesc(im) ; axis image off ;
  hold on ;
  vl_plotframe(frames) ;
  title(names{i}) ;
end

vl_figaspect(3/4) ;
vl_demo_print('covdet_detectors',.9) ;

% --------------------------------------------------------------------
%                                                        Custom frames
% --------------------------------------------------------------------

delta = 15 ;
xr = delta:delta:size(im,2)-delta+1 ;
yr = delta:delta:size(im,1)-delta+1 ;
[x,y] = meshgrid(xr,yr) ;
frames = [x(:)'; y(:)'] ;
frames(end+1,:) = delta/2 ;

[frames, patches] = vl_covdet(imgs, ...
                              'frames', frames, ...
                              'estimateAffineShape', true, ...
                              'estimateOrientation', true) ;

figure(7) ; clf ;
imagesc(im) ;
axis image off ; hold on ; colormap gray ;
vl_plotframe(frames) ;
vl_demo_print('covdet_custom_frames',.8) ;

% --------------------------------------------------------------------
%                                                         Scale spaces
% --------------------------------------------------------------------

[frames, descrs, info] = vl_covdet(imgs) ;

figure(8) ; clf ;
vl_plotss(info.gss) ;
colormap gray ;
vl_figaspect(2) ;
vl_demo_print('covdet_gss',.8) ;

figure(9) ; clf ;
vl_plotss(info.css) ;
colormap gray ;
vl_figaspect(2) ;
vl_demo_print('covdet_css',.8) ;

figure(10) ; clf ;
subplot(1,2,1) ;
hist([info.peakScores],10) ;
xlabel('Peak Score') ;
ylabel('Occurences') ;
grid on ;

subplot(1,2,2) ;
hist([info.edgeScores],10) ;
xlabel('Edge Score') ;
ylabel('Occurences') ;
grid on ;

vl_figaspect(2) ;
vl_demo_print('covdet_scores',.9) ;
