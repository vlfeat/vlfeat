function imdb = setupFMD(datasetDir, varargin)
% SETUPSCENE67    Setup Flickr Material Dataset
%    This is similar to SETUPCALTECH101(), with modifications to setup
%    the Flickr Material Dataset accroding to the standard
%    evaluation protocols.
%
%    See: SETUPCALTECH101().

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.lite = false ;
opts.seed = 1 ;
opts.numTrain = 50 ;
opts.numTest = 50 ;
opts.autoDownload = true ;
opts = vl_argparse(opts, varargin) ;

% Download and unpack
vl_xmkdir(datasetDir) ;
if exist(fullfile(datasetDir, 'image', 'wood'))
  % ok
elseif opts.autoDownload
  url = 'http://people.csail.mit.edu/celiu/CVPR2010/FMD/FMD.zip' ;
  fprintf('Downloading FMD data to ''%s''. This will take a while.', datasetDir) ;
  unzip(url, datasetDir) ;
else
  error('FMD not found in %s', datasetDir) ;
end

imdb = setupGeneric(fullfile(datasetDir,'image'), ...
  'numTrain', opts.numTrain, 'numVal', 0, 'numTest', opts.numTest,  ...
  'expectedNumClasses', 10, ...
  'seed', opts.seed, 'lite', opts.lite) ;
