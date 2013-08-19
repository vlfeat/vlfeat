function imdb = setupScene67(datasetDir, varargin)
% SETUPSCENE67    Setup MIT Scene 67 dataset
%    This is similar to SETUPGENERIC(), with modifications to setup
%    MIT Scene 67 according to the standard evaluation protocols. The
%    function supports only the LITE option.
%
%    See: SETUPGENERIC().

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.lite = false ;
opts = vl_argparse(opts, varargin) ;

% Download and unpack
vl_xmkdir(datasetDir) ;
if exist(fullfile(datasetDir, 'Images', 'airport_inside'))
  % ok
else
  url = 'http://groups.csail.mit.edu/vision/LabelMe/NewImages/indoorCVPR_09.tar' ;
  trainImagesUrl = 'http://web.mit.edu/torralba/www/TrainImages.txt' ;
  testImagesUrl = 'http://web.mit.edu/torralba/www/TestImages.txt' ;
  fprintf('Downloading MIT Scene 67 data to ''%s''. This will take a while.', datasetDir) ;
  urlwrite(trainImagesUrl, fullfile(datasetDir, 'TrainImages.txt')) ;
  urlwrite(testImagesUrl, fullfile(datasetDir, 'TestImages.txt')) ;
  untar(url, datasetDir) ;
end

% Construct image database imdb structure
imdb.meta.sets = {'train', 'val', 'test'} ;

trainNames = textread(fullfile(datasetDir, 'TrainImages.txt'),'%s','delimiter','\n') ;
testNames = textread(fullfile(datasetDir, 'TestImages.txt'),'%s','delimiter','\n') ;
names = [trainNames; testNames]' ;
sets = [ones(1,numel(trainNames)), 3*ones(1,numel(testNames))] ;
imdb.images.id = 1:numel(names) ;
[imdb.images.name,perm] = sort(names) ;
imdb.images.set = sets(perm) ;

a = regexp(imdb.images.name, '^([^/]+)*/.*$', 'tokens') ;
for i = 1:numel(names)
  a{i} = a{i}{1}{1} ;
end
[imdb.meta.classes, ~, imdb.images.class] = unique(a) ;
imdb.images.class = imdb.images.class(:)' ;
imdb.imageDir = fullfile(datasetDir, 'Images') ;

if opts.lite
  ok = {} ;
  for c = 1:3
    ok{end+1} = vl_colsubset(find(imdb.images.class == c & imdb.images.set == 1), 5) ;
    ok{end+1} = vl_colsubset(find(imdb.images.class == c & imdb.images.set == 3), 5) ;
  end
  ok = cat(2, ok{:}) ;
  imdb.meta.classes = imdb.meta.classes(1:3) ;
  imdb.images.id = imdb.images.id(ok) ;
  imdb.images.name = imdb.images.name(ok) ;
  imdb.images.set = imdb.images.set(ok) ;
  imdb.images.class = imdb.images.class(ok) ;
end
