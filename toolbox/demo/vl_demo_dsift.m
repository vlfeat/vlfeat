% VL_DEMO_DSIFT Demo: DSIFT

pfx = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------
I = imread(fullfile(vl_root,'data','a.jpg')) ;
I = single(vl_imdown(rgb2gray(I))) ;

% --------------------------------------------------------------------
%                                          Basic Dense SIFT invocation
% --------------------------------------------------------------------

binSize     = 4 ; % bin size in pixels
magnif      = 3 ; % bin size / keypoint scale
scale       = binSize / magnif ;
windowSize  = 2 ;

elaps_dsift = [] ;
elaps_dsift_fast = [] ;
err_dsift = [] ;
err_dsift_fast = [] ;

windowSizeRange = [1 1.2 1.5 1.7 2 2.5 5] ;
for wi = 1:length(windowSizeRange)
  windowSize = windowSizeRange(wi) ;

  tic ;
  [f, d] = vl_dsift(vl_imsmooth(I, sqrt(scale.^2 - .25)), ...
                    'size', binSize, ...
                    'step', 5, ...
                    'bounds', [20,20,210,140], ...
                    'windowsize', windowSize, ...
                    'floatdescriptors', ...
                    'verbose') ;
  elaps_dsift(wi) = toc ;

  tic ;
  [f, dfast] = vl_dsift(vl_imsmooth(I, sqrt(scale.^2 - .25)), ...
                        'size', binSize, ...
                        'step', 5, ...
                        'bounds', [20,20,210,140], ...
                        'windowsize', windowSize, ...
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
                     'floatdescriptors', ...
                     'windowsize', windowSize) ;
  elaps_sift(wi) = toc ;

  err_dsift(wi)      = mean(mean(abs(d     - d_)) ./ mean(d_)) * 100 ;
  err_dsift_fast(wi) = mean(mean(abs(dfast - d_)) ./ mean(d_)) * 100 ;
end

figure(1) ; clf ;
subplot(1,2,1) ; title('Descriptor  SIFT') ;
plot(windowSizeRange, [err_dsift ; err_dsift_fast]', 'linewidth', 3) ;
legend('DSIFT', 'DSIFT fast') ;
ylabel('Approx error (%)') ;
xlabel('windowSize parameter') ;

subplot(1,2,2) ; title('Speedup on regular SIFT') ;
plot(windowSizeRange, ...
     [elaps_sift ./ elaps_dsift ; ...
      elaps_sift ./ elaps_dsift_fast ; ...
      elaps_sift ./ elaps_sift], 'linewidth', 3) ;
legend('DSIFT', 'DSIFT fast', 'SIFT') ;
ylabel('Times faster') ;
xlabel('windowSize parameter') ;

