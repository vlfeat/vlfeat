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

conf.calDir = 'data/caltech-101' ;
conf.dataDir = 'data/' ;
conf.autoDownloadData = true ;
conf.numTrain = 15 ;
conf.numTest = 15 ;
conf.numClasses = 102 ;
conf.numWords = 600 ;
conf.numSpatialX = 4 ;
conf.numSpatialY = 4 ;
conf.quantizer = 'kdtree' ;
conf.svm.C = 10 ;
conf.svm.solver = 'pegasos' ;
conf.svm.biasMultiplier = 1 ;
conf.phowOpts = {'Step', 5} ;
conf.clobber = false ;
conf.tinyProblem = true ;
conf.prefix = 'baseline' ;
conf.randSeed = 1 ;

if conf.tinyProblem
  conf.prefix = 'tiny' ;
  conf.numClasses = 5 ;
  conf.numSpatialX = 2 ;
  conf.numSpatialY = 2 ;
  conf.numWords = 300 ;
  conf.phowOpts = {'Verbose', 2, 'Sizes', 7, 'Step', 3} ;
end

conf.vocabPath = fullfile(conf.dataDir, [conf.prefix '-vocab.mat']) ;
conf.histPath = fullfile(conf.dataDir, [conf.prefix '-hists.mat']) ;
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
model.numSpatialX = conf.numSpatialX ;
model.numSpatialY = conf.numSpatialY ;
model.quantizer = conf.quantizer ;
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

  % Quantize the descriptors to get the visual words
  vocab = vl_kmeans(descrs, conf.numWords, 'verbose', 'algorithm', 'elkan') ;
  save(conf.vocabPath, 'vocab') ;
else
  load(conf.vocabPath) ;
end

model.vocab = vocab ;

if strcmp(model.quantizer, 'kdtree')
  model.kdtree = vl_kdtreebuild(vocab) ;
end

% --------------------------------------------------------------------
%                                           Compute spatial histograms
% --------------------------------------------------------------------

if ~exist(conf.histPath) || conf.clobber
  hists = {} ;
  parfor ii = 1:length(images)
  % for ii = 1:length(images)
    fprintf('Processing %s (%.2f %%)\n', images{ii}, 100 * ii / length(images)) ;
    im = imread(fullfile(conf.calDir, images{ii})) ;
    hists{ii} = getImageDescriptor(model, im);
  end

  hists = cat(2, hists{:}) ;
  save(conf.histPath, 'hists') ;
else
  load(conf.histPath) ;
end

% --------------------------------------------------------------------
%                                                  Compute feature map
% --------------------------------------------------------------------

psix = vl_homkermap(hists, 1, .7, 'kchi2') ;

% --------------------------------------------------------------------
%                                                            Train SVM
% --------------------------------------------------------------------

if ~exist(conf.modelPath) || conf.clobber
  switch conf.svm.solver
    case 'pegasos'
      lambda = 1 / (conf.svm.C *  length(selTrain)) ;
      w = [] ;
      % for ci = 1:length(classes)
      parfor ci = 1:length(classes)
        perm = randperm(length(selTrain)) ;
        fprintf('Training model for class %s\n', classes{ci}) ;
        y = 2 * (imageClass(selTrain) == ci) - 1 ;
        w(:,ci) = vl_pegasos(psix(:,selTrain(perm)), ...
                             int8(y(perm)), lambda, ...
                             'NumIterations', 20/lambda, ...
                             'BiasMultiplier', conf.svm.biasMultiplier) ;
      end
    case 'liblinear'
      svm = train(imageClass(selTrain)', ...
                  sparse(double(psix(:,selTrain))),  ...
                  sprintf(' -s 3 -B %f -c %f', ...
                          conf.svm.biasMultiplier, conf.svm.C), ...
                  'col') ;
      w = svm.w' ;
  end

  model.b = conf.svm.biasMultiplier * w(end, :) ;
  model.w = w(1:end-1, :) ;

  save(conf.modelPath, 'model') ;
else
  load(conf.modelPath) ;
end

% --------------------------------------------------------------------
%                                                Test SVM and evaluate
% --------------------------------------------------------------------

% Estimate the class of the test images
scores = model.w' * psix + model.b' * ones(1,size(psix,2)) ;
[drop, imageEstClass] = max(scores, [], 1) ;

% Compute the confusion matrix
idx = sub2ind([length(classes), length(classes)], ...
              imageClass(selTest), imageEstClass(selTest)) ;
confus = zeros(length(classes)) ;
confus = vl_binsum(confus, ones(size(idx)), idx) ;

% Plots
figure(1) ; clf;
subplot(1,2,1) ;
imagesc(scores(:,[selTrain selTest])) ; title('Scores') ;
set(gca, 'ytick', 1:length(classes), 'yticklabel', classes) ;
subplot(1,2,2) ;
imagesc(confus) ;
title(sprintf('Confusion matrix (%.2f %% accuracy)', ...
              100 * mean(diag(confus)/conf.numTest) )) ;
print('-depsc2', [conf.resultPath '.ps']) ;
save([conf.resultPath '.mat'], 'confus', 'conf') ;

% -------------------------------------------------------------------------
function im = standarizeImage(im)
% -------------------------------------------------------------------------

im = im2single(im) ;
if size(im,1) > 480, im = imresize(im, [480 NaN]) ; end

% -------------------------------------------------------------------------
function hist = getImageDescriptor(model, im)
% -------------------------------------------------------------------------

im = standarizeImage(im) ;
width = size(im,2) ;
height = size(im,1) ;
numWords = size(model.vocab, 2) ;

% get PHOW features
[frames, descrs] = vl_phow(im, model.phowOpts{:}) ;

% quantize appearance
switch model.quantizer
  case 'vq'
    [drop, binsa] = min(vl_alldist(model.vocab, single(descrs)), [], 1) ;
  case 'kdtree'
    binsa = double(vl_kdtreequery(model.kdtree, model.vocab, ...
                                  single(descrs), ...
                                  'MaxComparisons', 15)) ;
end
    
% quantize location
width = size(im, 2) ;
height = size(im, 1) ;
binsx = vl_binsearch(linspace(1,width,model.numSpatialX+1), frames(1,:)) ;
binsy = vl_binsearch(linspace(1,height,model.numSpatialY+1), frames(2,:)) ;

% combined quantization
bins = sub2ind([model.numSpatialY, model.numSpatialX, numWords], ...
               binsy,binsx,binsa) ;
hist = zeros(model.numSpatialY * model.numSpatialX * numWords, 1) ;
hist = vl_binsum(hist, ones(size(bins)), bins) ;
hist = single(hist / sum(hist)) ;

% -------------------------------------------------------------------------
function [className, score] = classify(model, im)
% -------------------------------------------------------------------------

hist = getImageDescriptor(model, im) ;
psix = vl_homkermap(hist, 1, .7, 'kchi2') ;
scores = model.w' * psix + model.b' ;
[score, best] = max(scores) ;
className = model.classes{best} ;
