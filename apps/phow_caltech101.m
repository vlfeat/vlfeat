function phow_caltech101
% PHOW_CALTECH101 Image classification in the Caltech-101 dataset
%   A quick test should run out of the box by running PHOW_CALTECH101
%   from MATLAB (provied that VLFeat is correctly installed).
%
%   The program automatically downloads the Caltech-101 data from the
%   interned and decompresses it in CONF.CALDIR, which defaults to
%   'data/caltech-101'. Change this path to the desidred location, for
%   instance to point to an existing copy of the data.
%
%   Intermediate files are stored in the directory CONF.DATADIR.
%
%   To run on the entiere dataset, change CONF.TINYPROBLEM to
%   FALSE.
%
%   To run with a different number of training/testing images, change
%   CONF.NUMTRAIN and CONF.NUMTEST. By default 15 training images are
%   used, which should result in about 65% performance (this is quite
%   remarkable for a single descriptor).

% AUTORIGHTS

conf.calDir = 'data/caltech-101' ;
conf.dataDir = 'data/' ;
conf.autoDownloadData = true ;
conf.numTrain = 15 ;
conf.numTest = 15 ;
conf.numClasses = 102 ;
conf.numWords = 800 ;
conf.numSpatialX = 4 ;
conf.numSpatialY = 4 ;
conf.svm.C = 10 ;
conf.svm.solver = 'pegasos' ;
conf.phowOpts = {} ;
conf.clobber = false ;
conf.tinyProblem = false ;
conf.prefix = 'baseline' ;
conf.randSeed = 1 ;

if conf.tinyProblem
  conf.prefix = 'tiny' ;
  conf.numClasses = 5 ;
  conf.numSpatialX = 2 ;
  conf.numSpatialY = 2 ;
  conf.numWords = 300 ;
  conf.phowOpts = {'Verbose', 2, 'Sizes', 7, 'Step', 3, 'Color', true} ;
end

conf.vocabPath = fullfile(conf.dataDir, [conf.prefix '-vocab.mat']) ;
conf.histPath = fullfile(conf.dataDir, [conf.prefix '-hists.mat']) ;
conf.modelPath = fullfile(conf.dataDir, [conf.prefix '-model.mat']) ;
conf.resultPath = fullfile(conf.dataDir, [conf.prefix '-result.mat']) ;

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
       'Set conf.autoDownloadData=true to download the requried data.']) ;
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
classes = {classes(3:conf.numClasses+2-1).name} ;

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

% --------------------------------------------------------------------
%                                                     Train vocabulary
% --------------------------------------------------------------------

if ~exist(conf.vocabPath) || conf.clobber

  % Get some PHOW descriptos to train the dictionary
  selTrainFeats = vl_colsubset(selTrain, 30) ;
  descrs = {} ;
  for ii = 1:length(selTrainFeats)
    im = imread(fullfile(conf.calDir, images{ii})) ;
    im = imageStandarize(im) ;
    [drop, descrs{ii}] = vl_phow(im, conf.phowOpts{:}) ;
  end

  descrs = vl_colsubset(cat(2, descrs{:}), 10e4) ;
  descrs = single(descrs) ;

  % Quantize the descriptors to get the visual words
  words = vl_kmeans(descrs, conf.numWords, 'verbose', 'algorithm', 'elkan') ;
  save(conf.vocabPath, 'words') ;
else
  load(conf.vocabPath) ;
end


% --------------------------------------------------------------------
%                                           Compute spatial histograms
% --------------------------------------------------------------------

if ~exist(conf.histPath) || conf.clobber
  hists = {} ;
  % par
  for ii = 1:length(images)
    fprintf('Processing %s (%.2f %%)\n', images{ii}, 100 * ii / length(images)) ;
    im = imread(fullfile(conf.calDir, images{ii})) ;
    im = imageStandarize(im) ;
    [frames, descrs] = vl_phow(im, conf.phowOpts{:}) ;

    % quantize appearance
    [drop, binsa] = min(vl_alldist(words, single(descrs)), [], 1) ;

    % quantize location
    width = size(im, 2) ;
    height = size(im, 1) ;
    binsx = vl_binsearch(linspace(1,width,conf.numSpatialX+1), frames(1,:)) ;
    binsy = vl_binsearch(linspace(1,height,conf.numSpatialY+1), frames(2,:)) ;

    % combined quantization
    bins = sub2ind([conf.numSpatialY, conf.numSpatialX, conf.numWords], ...
                   binsy,binsx,binsa) ;
    hist = zeros(conf.numSpatialY * conf.numSpatialX * conf.numWords, 1) ;
    hist = vl_binsum(hist, ones(size(bins)), bins) ;
    hist = single(hist / sum(hist)) ;

    hists{ii} = hist ;
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

biasMultiplier = 1 ;

if ~exist(conf.modelPath) || conf.clobber
  switch conf.svm.solver
    case 'pegasos'
      lambda = 1 / (conf.svm.C *  length(selTrain)) ;
      models = [] ;
      for ci = 1:length(classes)
        perm = randperm(length(selTrain)) ;
        fprintf('Training model for class %s\n', classes{ci}) ;
        y = 2 * (imageClass(selTrain) == ci) - 1 ;
        models(:,ci) = vl_pegasos(psix(:,selTrain(perm)), ...
                                  int8(y(perm)), lambda, ...
                                  'NumIterations', 20/lambda, ...
                                  'BiasMultiplier', biasMultiplier) ;
      end
    case 'liblinear'
      model = train(imageClass(selTrain)', ...
                    sparse(double(psix(:,selTrain))),  ...
                    sprintf(' -s 3 -B 1 -c %f', conf.svm.C), ...
                    'col') ;
      models = model.w' ;
  end
  save(conf.modelPath, 'models') ;
else
  load(conf.modelPath) ;
end

% --------------------------------------------------------------------
%                                                Test SVM and evaluate
% --------------------------------------------------------------------

% Estimate the class of the test images
scores = [] ;
for ci = 1:length(classes)
  scores(ci, :) = models(1:end-1,ci)' * psix + models(end,ci) * biasMultiplier ;
end
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
function im = imageStandarize(im)
% -------------------------------------------------------------------------

im = im2single(im) ;
if size(im,1) > 480, im = imresize(im, [480 NaN]) ; end
