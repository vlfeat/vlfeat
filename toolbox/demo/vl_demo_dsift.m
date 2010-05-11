% VL_DEMO_DSIFT Demo: DSIFT

randn('state',0) ;
rand('state',0) ;

% read a test image
I = imread(fullfile(vl_root,'data','a.jpg')) ;
I = single(vl_imdown(rgb2gray(I))) ;

% --------------------------------------------------------------------
%                                                      Basic benchmark
% --------------------------------------------------------------------

binSize = 4 ; % bin size in pixels
magnif = 3 ; % bin size / keypoint scale

elaps_dsift = [] ;
elaps_dsift_fast = [] ;
err_dsift = [] ;
err_dsift_fast = [] ;

binSizeRange = [3 4 5 6] ;
for wi = 1:length(binSizeRange)
  binSize = binSizeRange(wi) ;
  scale = binSize / magnif ;

  tic ;
  [f, d] = vl_dsift(vl_imsmooth(I, sqrt(scale.^2 - .25)), ...
                    'size', binSize, ...
                    'step', 2, ...
                    'bounds', [20,20,210,140], ...
                    'floatdescriptors', ...
                    'verbose') ;
  elaps_dsift(wi) = toc ;

  tic ;
  [f, dfast] = vl_dsift(vl_imsmooth(I, sqrt(scale.^2 - .25)), ...
                        'size', binSize, ...
                        'step', 2, ...
                        'bounds', [20,20,210,140], ...
                        'floatdescriptors', ...
                        'fast', ...
                        'verbose') ;
  elaps_dsift_fast(wi) = toc ;

  numKeys = size(f, 2) ;
  f_ = [f ; ones(1, numKeys) * scale ; zeros(1, numKeys)] ;

  tic ;
  [f_, d_] = vl_sift(I, ...
                     'magnif', magnif, ...
                     'frames', f_, ...
                     'firstoctave', -1, ...
                     'levels', 5, ...
                     'floatdescriptors') ;
  elaps_sift(wi) = toc ;

  err_dsift(wi)      = mean(mean(abs(d     - d_)) ./ mean(d_)) * 100 ;
  err_dsift_fast(wi) = mean(mean(abs(dfast - d_)) ./ mean(d_)) * 100 ;
end

figure(1) ; clf ; title('Descriptor  SIFT') ;
plot(binSizeRange, [err_dsift ; err_dsift_fast]', 'linewidth', 3) ;
legend('DSIFT', 'DSIFT fast') ;
ylabel('Approx error (%)') ;
xlabel('binSize parameter') ;
grid on ;
axis square ;

figure(2) ; title('Speedup on regular SIFT') ;
plot(binSizeRange, ...
     [elaps_sift ./ elaps_dsift ; ...
      elaps_sift ./ elaps_dsift_fast ; ...
      elaps_sift ./ elaps_sift], 'linewidth', 3) ;
legend('DSIFT', 'DSIFT fast', 'SIFT') ;
ylabel('Speedup') ;
xlabel('binSize parameter') ;
grid on ;
axis square ;

figure(1) ; vl_demo_print('dsift_accuracy') ;
figure(2) ; vl_demo_print('dsift_speedup') ;
