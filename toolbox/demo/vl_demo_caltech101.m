function vl_demo_caltech101

datapath = '/data/caltech-101';
%feattype = 'sift';
%dicttype = 'kmeans';
feattype = 'dsift';
dicttype = 'kmeans';
featpath = sprintf('feat-%s', feattype);
dbpath   = sprintf('%s/db.mat', featpath);
dictpath = sprintf('%s/dict-%s.mat', featpath, dicttype);
histspath = sprintf('%s/hists-%s.mat', featpath, dicttype);

ntrain   = 15;
switch dicttype
case 'kmeans'
  nwords = 200;
  histsize = nwords;
case 'hikmeans'
  nwords = 8000;
  branch = 20;
  histsize = 0;
  depth = ceil(log(nwords)/log(branch));
  for i = 0:depth
    histsize = histsize + branch^i;
  end
end

rand('twister', 0);
%%%%%%%%%%%%%%%%%%%%%%%% List of images

if ~exist(featpath), mkdir(featpath); end

if ~exist(dbpath)
  fprintf('building dictionary\n');
  file_filt  = '.*\.(png|PNG|jpg|JPG|jpeg|JPEG|gif|GIF|bmp|BMP|pgm|PGM|pbm|PBM|ppm|PPM)$' ;
  dir_filt   = '.*' ;

  % Each folder is a category
  dir_list = dir(datapath) ;
  dir_list = {dir_list([dir_list.isdir]).name} ;
  db = struct();
  db.datapath = datapath;
  db.TRAIN = 0;
  db.TEST  = 1;
  c = 1;
  k = 1;
  for dn = dir_list
    dir_name = dn{1} ;
    if strcmp(dir_name,'.')  ||  strcmp(dir_name,'..') || ...
       isempty(regexp(dir_name,dir_filt))
      continue ;  
    end
    
    db.cat_names{c} = dir_name ;
  
    file_list = dir(fullfile(datapath, dir_name)) ;
    file_list = {file_list(~[file_list.isdir]).name} ;
   
    % Randomize list to make training and testing selection easier. 
    perm = randperm(length(file_list)) ;
    file_list = file_list(perm) ;
   
    n = 1; 
    for fn = file_list
      file_name = fn{1} ;
      % If the file is not an image, skip it.
      if(isempty(regexp(file_name, file_filt))), continue ; end 
          
      db.ims(k).seg        = k ;    
      db.ims(k).cat        = c ;
      db.ims(k).path       = fullfile(dir_name,file_name) ;
      if n <= ntrain
        db.ims(k).flag     = db.TRAIN;
      else
        db.ims(k).flag     = db.TEST;
      end

      k = k + 1 ;
      n = n + 1 ;
    end % next file
    c = c + 1 ;
  end % next directory
  save(dbpath, 'db');
end

db = load(dbpath);
db = db.db;

%%%%%%%%%%%%%%%%%%%%%%%%%%% Extract Features

if ~exist(featpath), mkdir(featpath); end

for i = 1:length(db.ims)
  [pathstr, name, ext] = fileparts(fullfile(featpath, db.ims(i).path));
  if ~exist(pathstr), mkdir(pathstr); end
 
  db.ims(i).featpath = fullfile(pathstr, [name '.mat']);

  % If the feature file exists, don't recompute it
  if exist(db.ims(i).featpath), continue; end

  I = imread(fullfile(db.datapath, db.ims(i).path));
  if size(I,3) > 1, I = rgb2gray(I); end

  switch feattype
  case 'dsift'
    I = imresize(I, 320/max(size(I)));
    I = im2single(I);
    [f d] = vl_dsift(I, 'size', 3, 'step', 3, 'fast');
  case 'sift'
    I = im2single(I);
    [f d] = vl_sift(I);
  case 'mser'
    error('Not implemented yet');
  end

  save(db.ims(i).featpath, 'f', 'd');
  fprintf('%d/%d done\r', i, length(db.ims));
end

percentkeep = 1.3/ntrain; % dsift, 150k features total
%percentkeep = 10;
%%%%%%%%%%%%%%%%%%%%%%%%%% Sample features, build dictionary
if ~exist(dictpath)
  labels = [];
  desc  = [];
  train = find([db.ims.flag] == db.TRAIN);
  for i = 1:length(train)
    s = train(i);
    feats = load(db.ims(s).featpath);
    perm = randperm(size(feats.d,2)) ;
    d = feats.d(:,perm);
    keep = round(size(d,2)*percentkeep/100);
    d = d(:,1:keep);
    desc  = [desc d];
    labels = [labels ones(1,size(d,2)).*db.ims(s).cat];
  end

  fprintf('building dictionary\n');
  switch dicttype
  case 'kmeans'
    dict = vl_ikmeans(desc, nwords);
  case 'hikmeans'
    dict = vl_hikmeans(desc, branch, nwords);
  end
  save(dictpath, 'dict');
end

%%%%%%%%%%%%%%%%%%%%%%%%% Construct histograms

if ~exist(histspath)
  fprintf('building histograms\n');
  dict = load(dictpath);
  dict = dict.dict;
  hists = zeros(histsize, length(db.ims));
  for i = 1:length(db.ims)
    feats = load(db.ims(i).featpath);
    switch dicttype
    case 'kmeans'
      hists(:,i) = vl_ikmeanshist(nwords, vl_ikmeanspush(feats.d, dict));
    case 'hikmeans'
      hists(:,i) = vl_hikmeanshist(dict, vl_hikmeanspush(dict, feats.d));
    end
    fprintf('histogram %d/%d done\r', i, length(db.ims));
  end
  save(histspath, 'hists');
end


%%%%%%%%%%%%%%%%%%%%%%%% Define some stuff
ncats = length(db.cat_names);
train = find([db.ims.flag] == db.TRAIN);
test  = find([db.ims.flag] == db.TEST);

hists = load(histspath);
hists = hists.hists;

%%%%%%%%%%%%%%%%%%%%%%%% Compute histogram stats for weights
N = length(train);

% idf weights are log (nimages / nimages_word_appears_in)
weights = sum(logical(hists(:,train)),2);
ind = find(weights);
weights(ind) = log(N ./ weights(ind));

% TODO: I think this should work, but doesn't
%usage = zeros(size(hists,1), ncats);
%labels = [db.ims(train).cat];
% usage = vl_binsum(usage, hists(:,train), labels, 2);

%%%%%%%%%%%%%%%%%%%%%%%% Classify

% weigh (tf * idf)
hists = hists .* repmat(weights, 1, size(hists,2));

% normalize
hists = hists ./ repmat(sum(hists), size(hists,1),1);
hists = single(hists); % TODO: does not support double yet

labels = [db.ims(train).cat];

if 0
  fprintf('building kdtree\n');
  forest = vl_kdtreebuild(hists(:,train));

  fprintf('querying kdtree\n');
  nn = 10;
  [index dist] = vl_kdtreequery(forest, hists(:,train), ...
                                hists(:,test),'NumNeighbors', nn);
  testlabels = labels(index);
  clabels = zeros(ncats,size(index,2));
  clabels = vl_binsum(clabels, ones(size(testlabels)), testlabels, 1);
  [maxv tlabels] = max(clabels);
  testlabels  = tlabels;
else
  testlabels = zeros(size(test));
  fprintf('computing distances\n');
  for i = 1:length(test)
    D = vl_alldist2(hists(:,train),hists(:,test(i)));
    [v ind] = min(D);
    testlabels(i) = labels(ind);
    fprintf('%d/%d done\r', i, length(test));
  end
  fprintf('\n');

end

truelabels = [db.ims(test).cat];


%%%%%%%%%%%%%%%%%%%%% Measure performance
conf  = zeros(ncats, ncats);
ind = sub2ind(size(conf), truelabels, testlabels);
conf  = vl_binsum(conf, ones(size(ind)), ind);
nconf = conf ./ repmat(sum(conf,2), 1, size(conf,2));

keyboard;
