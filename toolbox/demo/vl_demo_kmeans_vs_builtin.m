% VL_DEMO_KMEANS_VS_BUILTIN

clear elaps energy ;

dimension  = 128 ;
numData    = 1000 ;
numCenters = 10 ;
numTrials  = 10 ;

for trial = 1:numTrials
  X = rand(dimension, numData) ;

  tic ;
  [idx, C, e] = kmeans(X', numCenters) ;
  e = sum(e) ;
  elaps.builtin(trial) = toc ;
  energy.builtin(trial) = e ;

  tic ;
  [idx_, C_, e] = vl_kmeans(X, numCenters, 'verbose') ;
  elaps.vl(trial) = toc ;
  energy.vl(trial) = e ;

  tic ;
  [idx_, C_, e] = vl_kmeans(X, numCenters, 'initialization', 'plusplus') ;
  elaps.vlpp(trial) = toc ;
  energy.vlpp(trial) = e ;
end

figure(1) ; clf ;
subplot(1,2,1) ; title('Energy') ;
mu = [mean(elaps.builtin) mean(elaps.vl) mean(elaps.vlpp)] ;
st = [std(elaps.builtin) std(elaps.vl) std(elaps.vlpp)] ;
bar(mu) ; hold on ;
errorbar(mu, st, 'linestyle', 'none', 'color', 'r', 'linewidth', 4) ;

subplot(1,2,2) ;
mu = [mean(energy.builtin) mean(energy.vl) mean(energy.vlpp)] ;
st = [std(energy.builtin) std(energy.vl) std(energy.vlpp)] ;
bar(mu) ; hold on ;
errorbar(mu, st, 'linestyle', 'none', 'color', 'r', 'linewidth', 4) ;
