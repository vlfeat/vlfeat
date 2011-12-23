% VL_DEMO_KMEANS_INIT

if 0
  I = vl_imtest('box') ;
  [f, d] = vl_dsift(single(I) / 255, 'step', 1, 'size', 3) ;
  X = double(d) ;
  numCenters = 300 ;
else
  X = [] ;
  numCenters = 40 ;
  for i=1:numCenters
    X_ = randn(10,100) ;
    X_(i) = X_(i) + 100 ;
    X = [X X_] ;
  end
  numCenters = numCenters * 2 ;
end
numData    = size(X, 2) ;
dimension  = size(X, 1) ;


clear elaps energy ;

dimension  = 128 ;
numData    = 1000 ;
numCenters = 100 ;
numTrials  = 5 ;
numIters   = 100 ;

vl_twister(0) ;
for trial = 1:numTrials
  tic ;
  [C, idx, e] = vl_kmeans(X, numCenters, 'Verbose', ...
                          'Initialization', 'randsel', ...
                          'MaxNumIterations', numIters) ;
  elaps.vlrs(trial) = toc ;
  energy.vlrs(trial) = e ;

  tic ;
  [C_, idx_, e_] = vl_kmeans(X, numCenters, 'Verbose', ...
                          'Initialization', 'plusplus', ...
                          'MaxNumIterations', numIters) ;
  elaps.vlpp(trial) = toc ;
  energy.vlpp(trial) = e_ ;
end

figure(1) ; clf ;
subplot(1,2,1) ; title('Energy') ;
mu = [mean(elaps.vlrs) mean(elaps.vlpp)] ;
st = [std(elaps.vlrs) std(elaps.vlpp)] ;
bar(mu) ; hold on ;
errorbar(mu, st, 'linestyle', 'none', 'color', 'r', 'linewidth', 4) ;
title('Time') ;

subplot(1,2,2) ;
mu = [mean(energy.vlrs) mean(energy.vlpp)] ;
st = [std(energy.vlrs) std(energy.vlpp)] ;
bar(mu) ; hold on ;
errorbar(mu, st, 'linestyle', 'none', 'color', 'r', 'linewidth', 4) ;
title('Energy') ;
