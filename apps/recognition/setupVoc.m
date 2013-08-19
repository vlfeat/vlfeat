function imdb = setupVoc(datasetDir, varargin)
% SETUPVOC    Setup PASCAL VOC data
%   IMDB = SETUPVOC(DATASETDIR, 'EDITION', '2007')

opts.edition = '2007' ;
opts.lite = false ;
opts = vl_argparse(opts, varargin) ;

switch opts.edition
  case {'2007', '2008', '2010', '2011', '2012'}
  otherwise
    error('Unknown PASCAL VOC edition ''%s''.', opts.edition) ;
end

imdb.images.id = [] ;
imdb.images.set = uint8([]) ;
imdb.images.name = {} ;
imdb.images.size = zeros(2,0) ;
imdb.meta.sets = {'train', 'val', 'test'} ;
imdb.meta.classes = {'aeroplane', 'bicycle', 'bird', 'boat', 'bottle', 'bus', 'car', ...
  'cat', 'chair', 'cow', 'diningtable', 'dog', 'horse', 'motorbike', ...
  'person', 'pottedplant', 'sheep', 'sofa', 'train', 'tvmonitor'} ;
imdb.imageDir = fullfile(datasetDir, ['VOC', opts.edition], 'JPEGImages') ;

% Get the list of images
map = containers.Map() ;  
j = 0 ;
for si = 1:numel(imdb.meta.sets)
  setName = imdb.meta.sets{si} ;
  annoPath = fullfile(datasetDir, ['VOC' opts.edition], ...
    'ImageSets', 'Main', ...
    [setName '.txt']) ;
  fprintf('%s: reading %s\n', mfilename, annoPath) ;
  [names,labels] = textread(annoPath, '%s %f') ;
  for i=1:length(names)
    j = j + 1 ;
    map(names{i}) = j ;
    imdb.images.id(j) = j ;
    imdb.images.set(j) = si ;
    imdb.images.name{j} = [names{i} '.jpg'] ;
    %info = imfinfo(fullfile(imdb.imageDir, imdb.images.name{j})) ;
    %imdb.images.size(:,j) = [info.Width ; info.Height] ;
    %fprintf('\radded %s', imdb.images.name{j}) ;
  end
end

% Get class labels
for ci = 1:length(imdb.meta.classes)
  imdb.classes.imageIds{ci} = [] ;
  imdb.classes.difficult{ci} = false(0) ;
end
for si = 1:numel(imdb.meta.sets)
  for ci = 1:length(imdb.meta.classes)
    setName = imdb.meta.sets{si} ;
    className = imdb.meta.classes{ci} ;
    annoPath = fullfile(datasetDir, ['VOC' opts.edition], ...
      'ImageSets', 'Main', ...
      [className '_' setName '.txt']) ;
    fprintf('%s: reading %s\n', mfilename, annoPath) ;
    [names,labels] = textread(annoPath, '%s %f') ;
    for i = 1:numel(names)
      j = map(names{i}) ;
      if labels(i) >= 0
        imdb.classes.imageIds{ci}(end+1) = j ;
        imdb.classes.difficult{ci}(end+1) = (labels(i) == 0) ;
      end
    end
  end
end

if opts.lite
  ok = {} ;
  for c = 1:3
    trainIds = intersect(imdb.images.id(imdb.images.set == 1), imdb.classes.imageIds{c}) ;
    testIds = intersect(imdb.images.id(imdb.images.set == 3), imdb.classes.imageIds{c}) ;

    ok{end+1} = vl_colsubset(find(ismember(imdb.images.id, trainIds)), 5) ;
    ok{end+1} = vl_colsubset(find(ismember(imdb.images.id, testIds)), 5) ;
  end
  ok = unique(cat(2, ok{:})) ;
  imdb.meta.classes = imdb.meta.classes(1:3) ;
  imdb.classes.imageIds = imdb.classes.imageIds(1:3) ;
  imdb.classes.difficult = imdb.classes.difficult(1:3) ;
  imdb.images.id = imdb.images.id(ok) ;
  imdb.images.name = imdb.images.name(ok) ;
  imdb.images.set = imdb.images.set(ok) ;
  for c = 1:3
    ok = ismember(imdb.classes.imageIds{c}, imdb.images.id) ;
    imdb.classes.imageIds{c} = imdb.classes.imageIds{c}(ok) ;
    imdb.classes.difficult{c} = imdb.classes.difficult{c}(ok) ;
  end
end

