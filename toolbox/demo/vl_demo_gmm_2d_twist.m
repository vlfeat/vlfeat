%% data init
numClusters = 30;
dimension = 2;
cc=hsv(numClusters);
X = [];

a = 0.9;
t = linspace(0,4*pi,numClusters);
x = a*(t.*cos(t));
y = a*(t.*sin(t));
mu = [x;y];

Ns = linspace(1,numClusters,numClusters) * 20;

for i=1:numClusters
    add = randn(dimension,Ns(i)) + repmat(mu(:,i),1,Ns(i));
    X = [X, add*50];
end
X = single(X);

%% KMeans
elapsedKMEANS = tic;
[initMeans, assignments] = vl_kmeans(X, numClusters, ...
    'Verbose', ...
    'Algorithm','ann', ...
    'MaxNumIterations',5, ...
    'Distance','l2', ...
    'NumTrees',3, ...
    'MaxNumComparisons', 20);
elapsedKMEANS = toc(elapsedKMEANS);
fprintf('\n ---------- \n KMeans time - %f \n ---------- \n\n',elapsedKMEANS);

initSigmas = zeros(dimension,numClusters);
initWeights = zeros(1,numClusters);

figure(1) ; clf ;
subplot(3,1,1) ; hold on ;
for i=1:numClusters
    Xk = X(:,assignments==i);

    initWeights(i) = size(Xk,2) / numClusters;

    plot(Xk(1,:),Xk(2,:),'.','color',cc(i,:));
    if size(Xk,1) == 0 || size(Xk,2) == 0
        initSigmas(:,i) = diag(cov(X'));
    else
        initSigmas(:,i) = diag(cov(Xk'));
    end
end
axis equal
set(gca,'xtick',[],'ytick',[]);
axis off
title('GMM: KMeans intialization');

initSigmas = single(initSigmas);
initWeights = single(initWeights);

%% GMM kmeans
elapsedGMM = tic ;
[means,sigmas,weights,ll,posteriors] = vl_gmm(X, numClusters, ...
    'Initialization','custom', ...
    'InitMeans',initMeans, ...
    'InitCovariances',initSigmas, ...
    'InitPriors',initWeights, ...
    'verbose', ...
    'MaxNumIterations', 20);
elapsedGMM = toc(elapsedGMM);
fprintf('\n ---------- \n GMM time - %f \n ---------- \n',elapsedGMM);

subplot(3,1,2) ; hold on ;
[~,idx] = max(posteriors,[],1);
for i=1:numClusters
    plot(X(1,idx == i),X(2,idx == i),'.','color',cc(i,:));
    vl_plotframe([means(:,i)' sigmas(1,i) 0 sigmas(2,i)]);
end
title('GMM: Gaussian mixture - kmeans init');
set(gca,'xtick',[],'ytick',[]);
axis off
axis equal

%% GMM random
elapsedGMM = tic;
[means,sigmas,weights,ll,posteriors] = vl_gmm(X, numClusters, ...
    'Initialization','rand', ...
    'Verbose', ...
    'MaxNumIterations', 100);
elapsedGMM = toc(elapsedGMM);
fprintf('\n ---------- \n GMM time - %f \n ---------- \n',elapsedGMM);

subplot(3,1,3) ; hold on ;
[~,idx] = max(posteriors,[],1);
for i=1:numClusters
    plot(X(1,idx == i),X(2,idx == i),'.','color',cc(i,:));
    vl_plotframe([means(:,i)' sigmas(1,i) 0 sigmas(2,i)]);
end
title('GMM: Gaussian mixture - random init');
axis equal
set(gca,'xtick',[],'ytick',[]);
axis off
vl_demo_print('gmm_2d_shell',1.2);
