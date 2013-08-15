% VL_DEMO_GMM_2D_RAND  Demonstrate clustering points with a GMM

%% Create a random set of points
numPoints = 5000 ;
dimension = 2 ;
numClusters = 20 ;
data = rand(dimension, numPoints) ;

%% Learn a GMM: fit the points at maximum likelihood
vl_twister('state',0) ;
[means, covariances, priors] = ...
    vl_gmm(data, numClusters, ...
           'MaxNumIterations', 1000, ...
           'Verbose') ;

figure(1) ; clf ; hold on
plot(data(1,:),data(2,:),'r.');
for i=1:numClusters
  vl_plotframe([means(:,i)' covariances(1,i) 0 covariances(2,i)], ...
               'Color','blue','LineWidth',2);
end

title('GMM: Gaussian mixture initialized choosing random points') ;
axis equal ; axis off ;
vl_demo_print('gmm_2d_rand',0.6);

