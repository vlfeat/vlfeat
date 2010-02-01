function vl_demo_caltech101

datapath = '~/data/caltech-101';
%feattype = 'dsift';
feattype = 'sift';
dicttype = 'kmeans';
featpath = sprintf('feat-%s', feattype);
dictpath = sprintf('%s/dict-%s.mat', featpath, dicttype);
histspath = sprintf('%s/hists-%s.mat', featpath, dicttype);

ntrain   = 15;
nwords = 200;

rand('twister', 0);
%%%%%%%%%%%%%%%%%%%%%%%% List of images

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
        
    db.segs(k).seg        = k ;    
    db.segs(k).cat        = c ;
    db.segs(k).path       = fullfile(dir_name,file_name) ;
    if n < ntrain
      db.segs(k).flag     = db.TRAIN;
    else
      db.segs(k).flag     = db.TEST;
    end

    k = k + 1 ;
    n = n + 1 ;
  end % next file
  c = c + 1 ;
end % next directory

%%%%%%%%%%%%%%%%%%%%%%%%%%% Extract Features

if ~exist(featpath), mkdir(featpath); end

for i = 1:length(db.segs)
  [pathstr, name, ext] = fileparts(fullfile(featpath, db.segs(i).path));
  if ~exist(pathstr), mkdir(pathstr); end
 
  db.segs(i).featpath = fullfile(pathstr, [name '.mat']);

  % If the feature file exists, don't recompute it
  if exist(db.segs(i).featpath), continue; end

  I = imread(fullfile(db.datapath, db.segs(i).path));
  if size(I,3) > 1, I = rgb2gray(I); end

  switch feattype
  case 'dsift'
    I = imresize(I, 320/max(size(I)));
    I = im2single(I);
    [f d] = vl_dsift(I, 'size', 3, 'step', 3);
  case 'sift'
    I = im2single(I);
    [f d] = vl_sift(I);
  case 'mser'
    error('Not implemented yet');
  end

  save(db.segs(i).featpath, 'f', 'd');
  fprintf('%d/%d done\r', i, length(db.segs));
end

percentkeep = 10;
%%%%%%%%%%%%%%%%%%%%%%%%%% Sample features, build dictionary
if ~exist(dictpath)
  labels = [];
  desc  = [];
  train = find([db.segs.flag] == db.TRAIN);
  for i = 1:length(train)
    s = train(i);
    feats = load(db.segs(s).featpath);
    perm = randperm(size(feats.d,2)) ;
    d = feats.d(:,perm);
    keep = round(size(d,2)*percentkeep/100);
    d = d(:,1:keep);
    desc  = [desc d];
    labels = [labels ones(1,size(d,2)).*db.segs(s).cat];
  end

  fprintf('building dictionary\n');
  dict = vl_ikmeans(desc, nwords);
  save(dictpath, 'dict');
end

%%%%%%%%%%%%%%%%%%%%%%%%% Construct histograms

if ~exist(histspath)
  fprintf('building histograms\n');
  dict = load(dictpath);
  dict = dict.dict;
  hists = zeros(nwords, length(db.segs));
  for i = 1:length(db.segs)
    feats = load(db.segs(i).featpath);
    hists(:,i) = vl_ikmeanshist(nwords, vl_ikmeanspush(feats.d, dict));
    fprintf('histogram %d/%d done\r', i, length(db.segs));
  end
  save(histspath, 'hists');
end

%%%%%%%%%%%%%%%%%%%%%%%% Classify

hists = load(histspath);
hists = hists.hists;
% normalize
hists = hists ./ repmat(sum(hists), size(hists,1),1);
hists = single(hists); % TODO: does not support double yet

ncats = length(db.cat_names);
train = find([db.segs.flag] == db.TRAIN);
test  = find([db.segs.flag] == db.TEST);
fprintf('building kdtree\n');
forest = vl_kdtreebuild(hists(:,train));
labels = [db.segs(train).cat];

fprintf('querying kdtree\n');
nn = 10;
[index dist] = vl_kdtreequery(forest, hists(:,train), ...
                              hists(:,test),'NumNeighbors', nn);
testlabels = labels(index);
clabels = zeros(ncats,size(index,2));
clabels = vl_binsum(clabels, ones(size(testlabels)), testlabels, 1);
[maxv tlabels] = max(clabels);
testlabels  = tlabels;

truelabels = [db.segs(test).cat];

imagesc(vl_alldist2(hists,hists));

%%%%%%%%%%%%%%%%%%%%% Measure performance
conf  = zeros(ncats, ncats);
ind = sub2ind(size(conf), truelabels, testlabels);
conf  = vl_binsum(conf, ones(size(ind)), ind);
nconf = conf ./ repmat(sum(conf,2), 1, size(conf,2));

keyboard;
