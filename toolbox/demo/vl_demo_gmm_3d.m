%% data init
numClusters = 50;
numData = 500; %size of one cluster
dimension = 3;
mixtureDist = 15;
cc=hsv(numClusters);
X = [];
for i=1:numClusters
    X = [X, randn(dimension,numData)+repmat(rand(dimension,1)*mixtureDist,1,numData)];
end

%% kmeans
elapsedKMEANS = tic;
[initMeans, assignments] = vl_kmeans(X, numClusters, ...
    'verbose', ...
    'multithreading', 'parallel', ...
    'algorithm','ann', ...
    'MaxNumIterations',10, ...
    'distance','l2', ...
    'NumTrees',3, ...
    'MaxNumComparisons', 10);
elapsedKMEANS = toc(elapsedKMEANS);
fprintf('\n ---------- \n KMeans time - %f \n ---------- \n\n',elapsedKMEANS);

initSigmas = zeros(dimension,numClusters);
initWeights = zeros(1,numClusters);

figure('Name','GMM');
subplot(1,2,1);
hold on
for i=1:numClusters
    Xk = X(:,assignments==i);
    
    initWeights(i) = size(Xk,2) / numClusters;
    
    plot3(Xk(1,:),Xk(2,:),Xk(3,:),'.','color',cc(i,:));
    if size(Xk,1) == 0 || size(Xk,2) == 0
        initSigmas(:,i) = diag(cov(X'));
    else
        initSigmas(:,i) = diag(cov(Xk'));
    end
end
axis equal
xlabel('x');
ylabel('y');
zlabel('z');
title('GMM: KMeans intialization');

%% gmm
elapsedGMM = tic;
[means,sigmas,weights,ll,posteriors] = vl_gmm(X, numClusters, ...
    'initialization','custom', ...
    'InitMeans',initMeans, ...
    'InitSigmas',initSigmas, ...
    'InitWeights',initWeights, ...
    'verbose', ...
    'multithreading', 'parallel', ...
    'MaxNumIterations', 10);
elapsedGMM = toc(elapsedGMM);
fprintf('\n ---------- \n GMM time - %f \n ---------- \n',elapsedGMM);

subplot(1,2,2);
hold on
[~,idx] = max(posteriors',[],1);
for i=1:numClusters
    plot3(X(1,idx == i),X(2,idx == i),X(3,idx == i),'.','color',cc(i,:));
end
axis equal
xlabel('x');
ylabel('y');
zlabel('z');
title('GMM: Estimated gaussian mixture');
