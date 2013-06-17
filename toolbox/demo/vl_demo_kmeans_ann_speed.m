dimension  = 128 ;
numData    = 10000 ;
numCenters = 200 ;
numTrials  = 5;
numIterations = 5;
initialization = 'randsel';
distance = 'l2';
%initialization = 'plusplus';

X = [];
switch 1
    case 1
        % one random cluster
        X = randn(dimension,numData);
    case 2
        %several well refined clusters
        mixtureDist = 50;
        for i=1:numCenters
            X = [X randn(dimension,numData/numCenters)+repmat(rand(dimension,1)*mixtureDist, 1, numData/numCenters)];
        end
end

if dimension == 3
    figure
    plot3(X(1,:),X(2,:),X(3,:),'r.')
    axis equal
end

algs = {'Lloyd','Elkan','1/4','1/10','1/50'};

timesSerial = zeros(numTrials,5);
timesParallel = zeros(numTrials,5);

eSerial = zeros(numTrials,5);
eParallel = zeros(numTrials,5);

%serial trials
for trial = 1:numTrials
    %lloyd
    tic
    vl_threads(1) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','lloyd', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance);
    timesSerial(trial,1) = toc;
    eSerial(trial,1) = E;

    %elkan
    tic
    vl_threads(1) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','elkan', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance);
    timesSerial(trial,2) = toc;
    eSerial(trial,2) = E;

    %ann 1/4
    tic
    vl_threads(1) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','ann', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance, ...
        'NumTrees', 3, ...
        'MaxNumComparisons', ceil(numCenters/4));
    timesSerial(trial,3) = toc;
    eSerial(trial,3) = E;

    %ann 1/25
    tic
    vl_threads(1) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','ann', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance, ...
        'NumTrees', 3, ...
        'MaxNumComparisons', ceil(numCenters/10));
    timesSerial(trial,4) = toc;
    eSerial(trial,4) = E;

    %acc 1/100
    tic
    vl_threads(1) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','ann', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance, ...
        'NumTrees', 3, ...
        'MaxNumComparisons', ceil(numCenters/50));
    timesSerial(trial,5) = toc;
    eSerial(trial,5) = E;
end

timesSerial = sum(timesSerial,1)/numTrials;
eSerial = sum(eSerial,1)/numTrials;

figure
subplot(3,2,1)
bar(timesSerial);
set(gca,'xticklabel',algs);
set(gca,'FontSize',8),
xlabel('Algorithm');
ylabel('Time serial [s]');

subplot(3,2,2)
bar(eSerial);
set(gca,'xticklabel',algs);
set(gca,'FontSize',8),
xlabel('Algorithm');
ylabel('Energy serial');

%parallel trials
for trial = 1:numTrials
    %lloyd
    tic
    vl_threads(0) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','lloyd', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance);
    timesParallel(trial,1) = toc;
    eParallel(trial,1) = E;

    %elkan
    tic
    vl_threads(0) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','elkan', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance);
    timesParallel(trial,2) = toc;
    eParallel(trial,2) = E;

    %ann 1/4
    tic
    vl_threads(0) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','ann', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance, ...
        'NumTrees', 3, ...
        'MaxNumComparisons', ceil(numCenters/4));
    timesParallel(trial,3) = toc;
    eParallel(trial,3) = E;

    %ann 1/25
    tic
    vl_threads(0) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','ann', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance, ...
        'NumTrees', 3, ...
        'MaxNumComparisons', ceil(numCenters/10));
    timesParallel(trial,4) = toc;
    eParallel(trial,4) = E;

    %acc 1/100
    tic
    vl_threads(0) ;
    [C, A, E] = vl_kmeans(X, numCenters, ...
        'verbose', ...
        'algorithm','ann', ...
        'initialization', initialization, ...
        'MaxNumIterations', numIterations, ...
        'distance', distance, ...
        'NumTrees', 3, ...
        'MaxNumComparisons', ceil(numCenters/50));
    timesParallel(trial,5) = toc;
    eParallel(trial,5) = E;
end

timesParallel = sum(timesParallel,1)/numTrials;
eParallel = sum(eParallel,1)/numTrials;

subplot(3,2,3);
bar(timesParallel);
set(gca,'xticklabel',algs);
set(gca,'FontSize',8),
xlabel('Algorithm');
ylabel('Time parallel [s]');

subplot(3,2,4);
bar(eParallel);
set(gca,'xticklabel',algs);
set(gca,'FontSize',8),
xlabel('Algorithm');
ylabel('Energy parallel');

speedup = timesSerial./timesParallel;

subplot(3,2,5);
bar(speedup);
set(gca,'xticklabel',algs);
set(gca,'FontSize',8),
xlabel('Algorithm');
ylabel('Ser/Par speedup ratio');

vl_demo_print('kmeans_speed',0.75);
