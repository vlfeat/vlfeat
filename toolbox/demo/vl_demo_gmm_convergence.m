dimension = 128;
numData = 10000;
numClusters = 100;
numIterations = 5;
numIterationsKM = 3;

X = rand(dimension,numData);
numTrials = 2;

lls = zeros(numTrials,2);

for trial = 1:numTrials
% kmeans initialization
startKMEANS = tic;
[initMeans, assignments] = vl_kmeans(X, numClusters, ...
    'verbose', ...
    'multithreading', 'parallel', ...
    'algorithm','ann', ...
    'MaxNumIterations',numIterationsKM, ...
    'distance','l2', ...
    'NumTrees',3, ...
    'MaxNumComparisons', 5);
elapsedKMEANS = toc(startKMEANS);
fprintf('\n ---------- \n KMeans time - %f \n ---------- \n\n',elapsedKMEANS);

% compute initial gaussian parameters
initWeights = zeros(1,numClusters);
initSigmas = zeros(dimension,numClusters);
for i=1:numClusters
    Xk = X(:,assignments==i);

    initWeights(i) = size(Xk,2) / numClusters;

    if size(Xk,1) == 0 || size(Xk,2) == 0
        initSigmas(:,i) = diag(cov(X'));
    else
        initSigmas(:,i) = diag(cov(Xk'));
    end
end

% gmm with kmeans init
[means,sigmas,weights,ll,posteriors] = vl_gmm(X, numClusters, ...
    'initialization','custom', ...
    'InitMeans',initMeans, ...
    'InitSigmas',initSigmas, ...
    'InitWeights',initWeights, ...
    'verbose', ...
    'multithreading', 'parallel', ...
    'MaxNumIterations', numIterations);
elapsedGMM = toc(startKMEANS);
fprintf('\n ---------- \n GMM kmeans init time - %f \n ---------- \n\n',elapsedGMM);

lls(trial,1) = ll;

% gmm with random init
elapsedGMM = tic;
[means,sigmas,weights,ll,posteriors] = vl_gmm(X, numClusters, ...
    'initialization','rand', ...
    'verbose', ...
    'multithreading', 'parallel', ...
    'MaxNumIterations', numIterations);
elapsedGMM = toc(elapsedGMM);
fprintf('\n ---------- \n GMM random init time - %f \n ---------- \n\n',elapsedGMM);

lls(trial,2) = ll;

end

lls = sum(lls,1)/numTrials;
inits = {'KMeans','Random'};
disp(lls)

figure
bar(lls);
set(gca,'xticklabel',inits);
xlabel('Initialization type');
ylabel('LogLikelyhood');
title('GMM convergence test')







