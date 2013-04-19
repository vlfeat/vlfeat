function phow_caltech101
% PHOW_CALTECH101 Image classification in the Caltech-101 dataset
%   This program demonstrates how to use VLFeat to construct an image
%   classifier on the Caltech-101 data. The classifier uses PHOW
%   features (dense SIFT), spatial histograms of visual words, and a
%   Chi2 SVM. To speedup computation it uses VLFeat fast dense SIFT,
%   kd-trees, and homogeneous kernel map. The program also
%   demonstrates VLFeat PEGASOS SVM solver, although for this small
%   dataset other solvers such as LIBLINEAR can be more efficient.
%
%   By default 15 training images are used, which should result in
%   about 64% performance (a good performance considering that only a
%   single feature type is being used).
%
%   Call PHOW_CALTECH101 to train and test a classifier on a small
%   subset of the Caltech-101 data. Note that the program
%   automatically downloads a copy of the Caltech-101 data from the
%   Internet if it cannot find a local copy.
%
%   Edit the PHOW_CALTECH101 file to change the program configuration.
%
%   To run on the entire dataset change CONF.TINYPROBLEM to FALSE.
%
%   The Caltech-101 data is saved into CONF.CALDIR, which defaults to
%   'data/caltech-101'. Change this path to the desired location, for
%   instance to point to an existing copy of the Caltech-101 data.
%
%   The program can also be used to train a model on custom data by
%   pointing CONF.CALDIR to it. Just create a subdirectory for each
%   class and put the training images there. Make sure to adjust
%   CONF.NUMTRAIN accordingly.
%
%   Intermediate files are stored in the directory CONF.DATADIR. All
%   such files begin with the prefix CONF.PREFIX, which can be changed
%   to test different parameter settings without overriding previous
%   results.
%
%   The program saves the trained model in
%   <CONF.DATADIR>/<CONF.PREFIX>-model.mat. This model can be used to
%   test novel images independently of the Caltech data.
%
%     load('data/baseline-model.mat') ; # change to the model path
%     label = model.classify(model, im) ;
%

% AUTORIGHTS

conf.calDir = './data/caltech-101' ;
conf.dataDir = 'data-fish/' ;
conf.autoDownloadData = true ;
conf.numTrain = 15 ;
conf.numTest = 15 ;
conf.numClasses = 102 ;
conf.numWords = 600 ;
conf.encoder = 'gmm' ;
conf.svm.C = 10 ;

conf.svm.solver = 'sgd' ;
%conf.svm.solver = 'liblinear' ;
%conf.svm.solver = 'sdca' ;

conf.svm.biasMultiplier = 1 ;
conf.phowOpts = {'Step', 3} ;
conf.clobber = false ;
conf.tinyProblem = true ;
conf.prefix = 'baseline' ;
conf.randSeed = 1 ;

if conf.tinyProblem
  conf.prefix = 'tiny' ;
  conf.numClasses = 5 ;
  conf.numWords = 300 ;
  conf.phowOpts = {'Verbose', 2, 'Sizes', 7, 'Step', 5} ;
end

conf.vocabPath = fullfile(conf.dataDir, [conf.prefix '-vocab.mat']) ;
conf.fishPath = fullfile(conf.dataDir, [conf.prefix '-fishers.mat']) ;
conf.modelPath = fullfile(conf.dataDir, [conf.prefix '-model.mat']) ;
conf.resultPath = fullfile(conf.dataDir, [conf.prefix '-result']) ;

randn('state',conf.randSeed) ;
rand('state',conf.randSeed) ;
vl_twister('state',conf.randSeed) ;

% --------------------------------------------------------------------
%                                            Download Caltech-101 data
% --------------------------------------------------------------------

if ~exist(conf.calDir, 'dir') || ...
   (~exist(fullfile(conf.calDir, 'airplanes'),'dir') && ...
    ~exist(fullfile(conf.calDir, '101_ObjectCategories', 'airplanes')))
  if ~conf.autoDownloadData
    error(...
      ['Caltech-101 data not found. ' ...
       'Set conf.autoDownloadData=true to download the required data.']) ;
  end
  vl_xmkdir(conf.calDir) ;
  calUrl = ['http://www.vision.caltech.edu/Image_Datasets/' ...
    'Caltech101/101_ObjectCategories.tar.gz'] ;
  fprintf('Downloading Caltech-101 data to ''%s''. This will take a while.', conf.calDir) ;
  untar(calUrl, conf.calDir) ;
end

if ~exist(fullfile(conf.calDir, 'airplanes'),'dir')
  conf.calDir = fullfile(conf.calDir, '101_ObjectCategories') ;
end

% --------------------------------------------------------------------
%                                                           Setup data
% --------------------------------------------------------------------
classes = dir(conf.calDir) ;
classes = classes([classes.isdir]) ;
classes = {classes(3:conf.numClasses+2).name} ;

images = {} ;
imageClass = {} ;
for ci = 1:length(classes)
  ims = dir(fullfile(conf.calDir, classes{ci}, '*.jpg'))' ;
  ims = vl_colsubset(ims, conf.numTrain + conf.numTest) ;
  ims = cellfun(@(x)fullfile(classes{ci},x),{ims.name},'UniformOutput',false) ;
  images = {images{:}, ims{:}} ;
  imageClass{end+1} = ci * ones(1,length(ims)) ;
end
selTrain = find(mod(0:length(images)-1, conf.numTrain+conf.numTest) < conf.numTrain) ;
selTest = setdiff(1:length(images), selTrain) ;
imageClass = cat(2, imageClass{:}) ;

model.classes = classes ;
model.phowOpts = conf.phowOpts ;
model.vocab = [] ;
model.w = [] ;
model.b = [] ;
model.classify = @classify ;

% --------------------------------------------------------------------
%                                                     Train vocabulary
% --------------------------------------------------------------------

if ~exist(conf.vocabPath) || conf.clobber

  % Get some PHOW descriptors to train the dictionary
  selTrainFeats = vl_colsubset(selTrain, 30) ;
  descrs = {} ;
  %for ii = 1:length(selTrainFeats)
  parfor ii = 1:length(selTrainFeats)
    im = imread(fullfile(conf.calDir, images{selTrainFeats(ii)})) ;
    im = standarizeImage(im) ;
    [drop, descrs{ii}] = vl_phow(im, model.phowOpts{:}) ;
  end

  descrs = vl_colsubset(cat(2, descrs{:}), 10e4) ;
  descrs = single(descrs) ;

  %initialize GMM by kmeans
  init_mean = vl_kmeans(descrs, conf.numWords, ...
      'verbose', 'MaxNumComparisons', ceil(conf.numWords/10), ...
      'MaxNumIterations', 20, 'NumTrees',3, 'algorithm', ...
      'ann', 'Initialization', 'plusplus');
  
  fprintf('Computing initial variances and coefficients...\n');
  
  % compute hard assignments
  kd_tree = vl_kdtreebuild(init_mean, 'numTrees', 3) ;
  assign = vl_kdtreequery(kd_tree, init_mean, descrs);
  
  % mixing coefficients
  init_weights = single(vl_binsum(zeros(conf.numWords, 1), 1, double(assign)));
  init_weights = init_weights / sum(init_weights);
  
  % initial variances
  init_var = zeros(size(descrs, 1), conf.numWords, 'single');
  for i = 1:conf.numWords
      descrs_cluster = descrs(:, assign == i);
      init_var(:, i) = var(descrs_cluster, 0, 2);
  end
  
  %find GMM
  fprintf('Learning GMM model: Number of descriptors: %d; Number of visual words: %d\n',size(descrs,2),conf.numWords)
  tic
  [means,sigmas,weights] = vl_gmm(descrs, conf.numWords, ...
      'initialization','custom', ...
      'InitMeans',init_mean, ...
      'InitSigmas',init_var, ...
      'InitWeights',init_weights, ...
      'verbose', ...
      'multithreading', 'parallel', ...
      'MaxNumIterations', 30);
  gmmTime = toc;
  fprintf('gmm time: %f\n',gmmTime);
  
  vocab.means = means;
  vocab.sigmas = sigmas;
  vocab.weights = weights;
  
  save(conf.vocabPath, 'vocab') ;
else
  load(conf.vocabPath) ;
end

model.vocab = vocab ;

% --------------------------------------------------------------------
%                                              Compute Fisher encoding
% --------------------------------------------------------------------

if ~exist(conf.fishPath) || conf.clobber
  codes = {} ;
  parfor ii = 1:length(images)
  % for ii = 1:length(images)
    fprintf('Processing %s (%.2f %%)\n', images{ii}, 100 * ii / length(images)) ;
    im = imread(fullfile(conf.calDir, images{ii})) ;
    codes{ii} = getImageDescriptor(model, im);
  end

  codes = cat(2, codes{:}) ;
  save(conf.fishPath, 'codes') ;
else
  load(conf.fishPath) ;
end


% --------------------------------------------------------------------
%                                        Compute hellinger feature map
% --------------------------------------------------------------------

codes = sign(codes).*sqrt(abs(codes));

% --------------------------------------------------------------------
%                                                            Train SVM
% --------------------------------------------------------------------

if ~exist(conf.modelPath) || conf.clobber
  switch conf.svm.solver
    case 'sgd'
      lambda = 1 / (conf.svm.C *  length(selTrain)) ;
      w = [] ;
      parfor ci = 1:length(classes)
        perm = randperm(length(selTrain)) ;
        fprintf('Training model for class %s\n', classes{ci}) ;
        y = 2 * (imageClass(selTrain) == ci) - 1 ;

        data = vl_maketrainingset(codes(:,selTrain(perm)), int8(y(perm))) ;
        [w(:,ci) b(ci) info] = vl_svmtrain(data, lambda, ...
                                        'MaxIterations', 50/lambda, ...
                                        'BiasMultiplier', conf.svm.biasMultiplier);
      end
    case 'sdca'
      lambda = 1 / (conf.svm.C *  length(selTrain)) ;
      w = [] ;
      parfor ci = 1:length(classes)
        perm = randperm(length(selTrain)) ;
        fprintf('Training model for class %s\n', classes{ci}) ;
        y = 2 * (imageClass(selTrain) == ci) - 1 ;
        data = vl_maketrainingset(codes(:,selTrain(perm)), int8(y(perm))) ;
        [w(:,ci) b(ci) info] = vl_svmtrain(data, lambda, 'DCA',...
                                        'MaxIterations', 5/lambda, ...
                                        'BiasMultiplier', conf.svm.biasMultiplier,...
                                        'Epsilon',0.00,...
                                        'OnlineSetting',logical(1));
      end
    case 'liblinear'
      svm = train(imageClass(selTrain)', ...
                  sparse(double(codes(:,selTrain))),  ...
                  sprintf(' -s 3 -B %f -c %f', ...
                          conf.svm.biasMultiplier, conf.svm.C), ...
                  'col') ;
      w = svm.w(:,1:end-1)' ;
      b =  svm.w(:,end)' ;
  end

  model.b = conf.svm.biasMultiplier * b ;
  model.w = w ;

  save(conf.modelPath, 'model') ;
else
  load(conf.modelPath) ;
end

% --------------------------------------------------------------------
%                                                Test SVM and evaluate
% --------------------------------------------------------------------

% Estimate the class of the test images
scores = model.w' * codes + model.b' * ones(1,size(codes,2)) ;
[drop, imageEstClass] = max(scores, [], 1) ;

% Compute the confusion matrix
idx = sub2ind([length(classes), length(classes)], ...
              imageClass(selTest), imageEstClass(selTest)) ;
confus = zeros(length(classes)) ;
confus = vl_binsum(confus, ones(size(idx)), idx) ;

% Plots
figure(1) ; clf;
subplot(1,2,1) ;
imagesc(scores(:,[selTrain selTest])) ; title('Scores','FontSize',16) ;
set(gca, 'ytick', 1:length(classes), 'yticklabel', classes) ;
set(gca,'FontSize',14)
subplot(1,2,2) ;
imagesc(confus) ;
title(sprintf('Confusion matrix (%.2f %% acc)', ...
              100 * mean(diag(confus)/conf.numTest) ),'FontSize',14) ;
set(gca,'FontSize',16)

print('-depsc2', [conf.resultPath '.ps']) ;
save([conf.resultPath '.mat'], 'confus', 'conf') ;

% -------------------------------------------------------------------------
function im = standarizeImage(im)
% -------------------------------------------------------------------------

im = im2single(im) ;
if size(im,1) > 480, im = imresize(im, [480 NaN]) ; end

% -------------------------------------------------------------------------
function enc = getImageDescriptor(model, im)
% -------------------------------------------------------------------------

im = standarizeImage(im) ;

% get PHOW features
[drop, descrs] = vl_phow(im, model.phowOpts{:}) ;
size(descrs)
% quantize appearance
enc = vl_fisher(single(descrs),model.vocab.means,model.vocab.sigmas,model.vocab.weights);
size(enc)
%normalize hist
enc = enc/sum(abs(enc));



% -------------------------------------------------------------------------
function [className, score] = classify(model, im)
% -------------------------------------------------------------------------

hist = getImageDescriptor(model, im) ;
psix = vl_homkermap(hist, 1, 'kchi2', 'period', .7) ;
scores = model.w' * psix + model.b' ;
[score, best] = max(scores) ;
className = model.classes{best} ;
