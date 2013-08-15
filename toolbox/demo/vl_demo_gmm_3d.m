function vl_demo_gmm_3d
% VL_DEMO_GMM_3D   Show how to initialize GMM learning with a custom set of modes

%% Initialize points clustered in 3D
numClusters = 5 ;
dimension = 3 ;
cc = hsv(numClusters) ;
randn('state',0) ;
X = [] ;
for i=1:numClusters
  delta = 5*[i;0;0] ;
  %num = ceil(1000 / i) ;
  num = 500 ;
  s = diag([1,i, 1]) ;
  X = [X, bsxfun(@plus, s*randn(dimension, num), delta)] ;
end

%% Cluster using KMeans (ANN algorithm)
elapsedKMEANS = tic ;
vl_twister('state',0) ;
[initMeans, assignments] = vl_kmeans(X, numClusters, ...
                                     'Verbose', ...
                                     'Algorithm', 'ann', ...
                                     'MaxNumIterations', 10, ...
                                     'Distance', 'l2', ...
                                     'NumTrees', 3, ...
                                     'MaxNumComparisons', 10);
elapsedKMEANS = toc(elapsedKMEANS) ;
fprintf('\n ---------- \n KMeans time - %f \n ---------- \n\n',elapsedKMEANS) ;

%% Create initial GMM modes from KMeans clusters
initSigmas = zeros(dimension,numClusters);
initWeights = zeros(1,numClusters);

figure(1) ; clf ;
subplot(1,2,1) ; hold on ;

for i=1:numClusters
  Xk = X(:,assignments==i) ;
  initWeights(i) = size(Xk,2) / numClusters ;

  plot3(Xk(1,:),Xk(2,:),Xk(3,:),'.','color',cc(i,:)) ;
  if size(Xk,1) == 0 || size(Xk,2) == 0
    initSigmas(:,i) = diag(cov(X'));
  else
    initSigmas(:,i) = diag(cov(Xk'));
  end
end

axis equal ;
xlabel('x') ; ylabel('y') ; zlabel('z') ;
title('GMM: KMeans intialization') ;

%% Fit a GMM model to the maximum likelihood
elapsedGMM = tic ;
[means,sigmas,weights,ll,posteriors] = vl_gmm(X, numClusters, ...
                                              'Initialization','custom', ...
                                              'InitMeans',initMeans, ...
                                              'InitCovariances',initSigmas, ...
                                              'InitPriors',initWeights, ...
                                              'Verbose', ...
                                              'MaxNumIterations', 100) ;
elapsedGMM = toc(elapsedGMM);
fprintf('\n ---------- \n GMM time - %f \n ---------- \n',elapsedGMM) ;

subplot(1,2,2) ; hold on ;
[~,idx] = max(posteriors,[],1);
for i=1:numClusters
  plot3(X(1,idx == i),X(2,idx == i),X(3,idx == i),'.','color',cc(i,:)) ;
end

axis equal ;
xlabel('x') ; ylabel('y') ; zlabel('z') ;
title('GMM: Estimated Gaussian mixture') ;
