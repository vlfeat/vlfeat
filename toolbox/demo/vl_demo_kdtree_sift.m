function vl_demo_kdtree_sift
% VL_DEMO_KDTREE_SIFT
%   Demonstrates the use of a kd-tree forest to match SIFT
%   features. If FLANN is present, this function runs a comparison
%   against it.

% AUTORIGHS

rand('state',0) ;
randn('state',0);

do_median = 0 ;
do_mean = 1 ;

% try to setup flann
if ~exist('flann_search', 'file')
  if exist(fullfile(vl_root, 'opt', 'flann', 'build', 'matlab'))
    addpath(fullfile(vl_root, 'opt', 'flann', 'build', 'matlab')) ;
  end
end
do_flann = exist('nearest_neighbors') == 3 ;
if ~do_flann
  warning('FLANN not found. Comparison disabled.') ;
end

maxNumComparisonsRange = [1 10 50 100 200 300 400] ;
numTreesRange = [1 2 5 10] ;

% get data (SIFT features)
im1 = imread(fullfile(vl_root, 'data', 'a.jpg')) ;
im2 = imread(fullfile(vl_root, 'data', 'b.jpg')) ;
im1 = single(rgb2gray(im1)) ;
im2 = single(rgb2gray(im2)) ;
[f1,d1] = vl_sift(im1,'firstoctave',-1,'floatdescriptors','verbose') ;
[f2,d2] = vl_sift(im2,'firstoctave',-1,'floatdescriptors','verbose') ;

% add some noise to make matches unique
d1 = single(d1) + rand(size(d1)) ;
d2 = single(d2) + rand(size(d2)) ;

% match exhaustively to get the ground truth
elapsedDirect = tic ;
D = vl_alldist(d1,d2) ;
[drop, best] = min(D, [], 1) ;
elapsedDirect = toc(elapsedDirect) ;

for ti=1:length(numTreesRange)
  for vi=1:length(maxNumComparisonsRange)
    v = maxNumComparisonsRange(vi) ;
    t = numTreesRange(ti) ;

    if do_median
      tic ;
      kdtree = vl_kdtreebuild(d1, ...
                              'verbose', ...
                              'thresholdmethod', 'median', ...
                              'numtrees', t) ;
      [i, d] = vl_kdtreequery(kdtree, d1, d2, ...
                              'verbose', ...
                              'maxcomparisons',v) ;
      elapsedKD_median(vi,ti) = toc ;
      errors_median(vi,ti) = sum(double(i) ~= best) / length(best) ;
      errorsD_median(vi,ti) = mean(abs(d - drop) ./ drop) ;
    end

    if do_mean
      tic ;
      kdtree = vl_kdtreebuild(d1, ...
                              'verbose', ...
                              'thresholdmethod', 'mean', ...
                              'numtrees', t) ;
      %kdtree = readflann(kdtree, '/tmp/flann.txt') ;
      %checkx(kdtree, d1, 1, 1) ;
      [i, d] = vl_kdtreequery(kdtree, d1, d2, ...
                              'verbose', ...
                              'maxcomparisons', v) ;
      elapsedKD_mean(vi,ti) = toc ;
      errors_mean(vi,ti) = sum(double(i) ~= best) / length(best) ;
      errorsD_mean(vi,ti) = mean(abs(d - drop) ./ drop) ;
    end

    if do_flann
      tic ;
      [i, d] = flann_search(d1, d2, 1, struct('algorithm','kdtree', ...
                                              'trees', t, ...
                                              'checks', v));
      ifla = i ;
      elapsedKD_flann(vi,ti) = toc;
      errors_flann(vi,ti) = sum(i ~= best) / length(best) ;
      errorsD_flann(vi,ti) = mean(abs(d - drop) ./ drop) ;
    end
  end
end

figure(1) ; clf ;
leg = {} ;
hnd = [] ;
sty = {{'color','r'},{'color','g'},...
       {'color','b'},{'color','c'},...
       {'color','k'}} ;

for ti=1:length(numTreesRange)
  s = sty{mod(ti,length(sty))+1} ;

  if do_median
    h1=loglog(elapsedDirect ./ elapsedKD_median(:,ti),100*errors_median(:,ti),'-*',s{:}) ;  hold on ;
    leg{end+1} = sprintf('VLFeat median (%d tr.)', numTreesRange(ti)) ;
    hnd(end+1) = h1 ;
  end

  if do_mean
    h2=loglog(elapsedDirect ./ elapsedKD_mean(:,ti), 100*errors_mean(:,ti), '-o',s{:}) ;  hold on ;
    leg{end+1} = sprintf('VLFeat (%d tr.)', numTreesRange(ti)) ;
    hnd(end+1) = h2 ;
  end

  if do_flann
    h3=loglog(elapsedDirect ./ elapsedKD_flann(:,ti), 100*errors_flann(:,ti), '+--',s{:}) ;  hold on ;
    leg{end+1} = sprintf('FLANN (%d tr.)', numTreesRange(ti)) ;
    hnd(end+1) = h3 ;
  end
end
set([hnd], 'linewidth', 2) ;
xlabel('speedup over linear search (log times)') ;
ylabel('percentage of incorrect matches (%)') ;
h=legend(hnd, leg{:}, 'location', 'southeast') ;
set(h,'fontsize',8) ;
grid on ;
axis square ;
vl_demo_print('kdtree_sift_incorrect',.6) ;

figure(2) ; clf ;
leg = {} ;
hnd = [] ;
for ti=1:length(numTreesRange)
  s = sty{mod(ti,length(sty))+1} ;

  if do_median
    h1=loglog(elapsedDirect ./ elapsedKD_median(:,ti),100*errorsD_median(:,ti),'*-',s{:}) ; hold on ;
    leg{end+1} = sprintf('VLFeat median (%d tr.)', numTreesRange(ti)) ;
    hnd(end+1) = h1 ;
  end

  if do_mean
    h2=loglog(elapsedDirect ./ elapsedKD_mean(:,ti), 100*errorsD_mean(:,ti), 'o-',s{:}) ; hold on ;
    leg{end+1} = sprintf('VLFeat (%d tr.)', numTreesRange(ti)) ;
    hnd(end+1) = h2 ;
  end

  if do_flann
    h3=loglog(elapsedDirect ./ elapsedKD_flann(:,ti), 100*errorsD_flann(:,ti), '+--',s{:}) ; hold on ;
    leg{end+1} = sprintf('FLANN (%d tr.)', numTreesRange(ti)) ;
    hnd(end+1) = h3 ;
  end
end
set([hnd], 'linewidth', 2) ;
xlabel('speedup over linear search (log times)') ;
ylabel('relative overestimation of minmium distannce (%)') ;
h=legend(hnd, leg{:}, 'location', 'southeast') ;
set(h,'fontsize',8) ;
grid on ;
axis square ;
vl_demo_print('kdtree_sift_distortion',.6) ;

% --------------------------------------------------------------------
function checkx(kdtree, X, t, n, mib, mab)
% --------------------------------------------------------------------

if nargin <= 4
  mib = -inf * ones(size(X,1),1) ;
  mab = +inf * ones(size(X,1),1) ;
end

lc = kdtree.trees(t).nodes.lowerChild(n) ;
uc = kdtree.trees(t).nodes.upperChild(n) ;

if lc < 0
  for i=-lc:-uc-1
    di = kdtree.trees(t).dataIndex(i) ;
    if any(X(:,di) > mab)
      error('a') ;
    end
    if any(X(:,di) < mib)
      error('b') ;
    end
  end
  return
end

i = kdtree.trees(t).nodes.splitDimension(n) ;
v = kdtree.trees(t).nodes.splitThreshold(n) ;

mab_ = mab ;
mab_(i) = min(mab(i), v) ;
checkx(kdtree, X, t, lc, mib, mab_) ;

mib_ = mib ;
mib_(i) = max(mib(i), v) ;
checkx(kdtree, X, t, uc, mib_, mab) ;

% --------------------------------------------------------------------
function kdtree = readflann(kdtree, path)
% --------------------------------------------------------------------

data = textread(path)' ;

for i=1:size(data,2)
  nodeIds = data(1,:) ;
  ni = find(nodeIds == data(1,i)) ;
  if ~isnan(data(2,i))
    % internal node
    li = find(nodeIds == data(4,i)) ;
    ri = find(nodeIds == data(5,i)) ;
    kdtree.trees(1).nodes.lowerChild(ni) = int32(li) ;
    kdtree.trees(1).nodes.upperChild(ni) = int32(ri) ;
    kdtree.trees(1).nodes.splitThreshold(ni) = single(data(2,i)) ;
    kdtree.trees(1).nodes.splitDimension(ni) = single(data(3,i)+1) ;
  else
    di = data(3,i) + 1 ;
    kdtree.trees(1).nodes.lowerChild(ni) = int32(- di) ;
    kdtree.trees(1).nodes.upperChild(ni) = int32(- di - 1) ;
  end
  kdtree.trees(1).dataIndex = uint32(1:kdtree.numData) ;
end

