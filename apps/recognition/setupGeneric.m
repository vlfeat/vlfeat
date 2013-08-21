function imdb = setupGeneric(datasetDir, varargin)
% SETUPGENERIC   Setup a Caltech-101-like dataset
%    IMDB = SETUPGENERIC(DATASETDIR) initializes a IMDB structure
%    representing the dataset located at DATASETDIR. The dataset
%    is supposed to be organized similarly to Caltech-101, i.e.
%    to have one directory per image class, with a number of
%    image files in each directory.
%
%    SETUPGENERIC(..., 'OPT', VAL, ...) accepts the following
%    options:
%
%    Lite:: false
%      If set to TRUE, use at most 3 classes and at most 5 images
%      in each of TRAIN, VAL, and TEST.
%
%    Seed:: 1
%      The random seed used to generate the partitions.
%
%    NumTrain:: 50
%       Maximum number of training images per class.
%
%    NumVal:: 0
%       Maximum number of validation images per class.
%
%    NumTest:: 50
%       Maximum number of test images per class.
%
%    ExpectedNumClasses:: []
%       If set to a number, generate an error if the number
%       of classes found in DATASETDIR differs. This is only
%       used as a sanity check.
%
%   IMDB is a structure with the following fields:
%
%   IMDB.IMAGES::
%      A structure of arrays representing the images in the
%      dataset.
%
%   IMDB.IMAGES.ID::
%      Vector of image numeric IDs.
%
%   IMDB.IMAGES.NAME::
%      Cell array with the image names.
%
%   IMDB.IMAGES.SET::
%      Vector of subset IDs for each image (train, val, test).
%
%   IMDB.IMAGES.CLASS::
%      Vector of class IDs for each image.
%
%   IMDB.META::
%      A substructure with meta information on the data.
%
%   IMDB.META.CLASSES::
%      Cell array of class names.
%
%   IMDB.IMAGEDIR::
%      Image directory. Join the image name to this directory to
%      access an image file.

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.lite = false ;
opts.numTrain = 50 ;
opts.numVal = 0 ;
opts.numTest = 50 ;
opts.seed = 1 ;
opts.expectedNumClasses = [] ;
opts = vl_argparse(opts, varargin) ;

% Construct image database imdb structure
imdb.meta.sets = {'train', 'val', 'test'} ;
names = dir(datasetDir) ;
names = {names([names.isdir]).name} ;
names = setdiff(names, {'.', '..'}) ;
imdb.meta.classes = names ;

names = {} ;
classes = {} ;
for c = 1:numel(imdb.meta.classes)
  class = imdb.meta.classes{c} ;
  tmp = dir(fullfile(datasetDir, [class filesep '*.jpg'])) ;
  names{c} = strcat([class filesep], {tmp.name}) ;
  classes{c} = repmat(c, 1, numel(names{c})) ;
end

names = cat(2,names{:}) ;
classes = cat(2,classes{:}) ;
sets = zeros(1,numel(names)) ;
ids = 1:numel(names) ;

numClasses = numel(imdb.meta.classes) ;
if ~isempty(opts.expectedNumClasses) && numClasses ~= opts.expectedNumClasses
  error('Expected %d classes in image database at %s.', opts.expectedNumClasses, datasetDir) ;
end

for c = 1:numClasses
  sel = find(classes == c) ;
  randn('state', opts.seed) ;
  rand('state', opts.seed) ;
  selTrain = vl_colsubset(sel, opts.numTrain) ;
  selVal = vl_colsubset(setdiff(sel, selTrain), opts.numVal) ;
  selTest = vl_colsubset(setdiff(sel, [selTrain selVal]), opts.numTest) ;
  sets(selTrain) = 1 ;
  sets(selVal) = 2 ;
  sets(selTest) = 3 ;
end

ok = find(sets ~= 0) ;
imdb.images.id = ids(ok) ;
imdb.images.name = names(ok) ;
imdb.images.set = sets(ok) ;
imdb.images.class = classes(ok) ;
imdb.imageDir = datasetDir ;

if opts.lite
  ok = {} ;
  for c = 1:3
    ok{end+1} = vl_colsubset(find(imdb.images.class == c & imdb.images.set == 1), 5) ;
    ok{end+1} = vl_colsubset(find(imdb.images.class == c & imdb.images.set == 2), 5) ;
    ok{end+1} = vl_colsubset(find(imdb.images.class == c & imdb.images.set == 3), 5) ;
  end
  ok = cat(2, ok{:}) ;
  imdb.meta.classes = imdb.meta.classes(1:3) ;
  imdb.images.id = imdb.images.id(ok) ;
  imdb.images.name = imdb.images.name(ok) ;
  imdb.images.set = imdb.images.set(ok) ;
  imdb.images.class = imdb.images.class(ok) ;
end
