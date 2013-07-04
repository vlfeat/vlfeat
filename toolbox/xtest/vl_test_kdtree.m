function results = vl_test_kdtree(varargin)
% VL_TEST_KDTREE
vl_test_init ;

function s = setup()
randn('state',0) ;
s.X = single(randn(10, 1000)) ;
s.Q = single(randn(10, 10)) ;

function test_nearest(s)
for tmethod = {'median', 'mean'}
  for type = {@single, @double}
    conv = type{1} ;
    tmethod = char(tmethod) ;

    X = conv(s.X) ;
    Q = conv(s.Q) ;
    tree = vl_kdtreebuild(X,'ThresholdMethod', tmethod) ;
    [nn, d2] = vl_kdtreequery(tree, X, Q) ;

    D2 = vl_alldist2(X, Q, 'l2') ;
    [d2_, nn_] = min(D2) ;

    vl_assert_equal(...
      nn,uint32(nn_),...
      'incorrect nns: type=%s th. method=%s', func2str(conv), tmethod) ;
    vl_assert_almost_equal(...
      d2,d2_,...
      'incorrect distances: type=%s th. method=%s', func2str(conv), tmethod) ;
  end
end

function test_nearests(s)
numNeighbors = 7 ;
tree = vl_kdtreebuild(s.X) ;
[nn, d2] = vl_kdtreequery(tree, s.X, s.Q, ...
                          'numNeighbors', numNeighbors) ;

D2 = vl_alldist2(s.X, s.Q, 'l2') ;
[d2_, nn_] = sort(D2) ;
d2_ = d2_(1:numNeighbors, :) ;
nn_ = nn_(1:numNeighbors, :) ;

vl_assert_equal(nn,uint32(nn_)) ;
vl_assert_almost_equal(d2,d2_) ;

function test_ann(s)
vl_twister('state', 1) ;
numNeighbors = 7 ;
maxComparisons = numNeighbors * 50 ;
tree = vl_kdtreebuild(s.X) ;
[nn, d2] = vl_kdtreequery(tree, s.X, s.Q, ...
                          'numNeighbors', numNeighbors, ...
                          'maxComparisons', maxComparisons) ;

D2 = vl_alldist2(s.X, s.Q, 'l2') ;
[d2_, nn_] = sort(D2) ;
d2_ = d2_(1:numNeighbors, :) ;
nn_ = nn_(1:numNeighbors, :) ;

for i=1:size(s.Q,2)
  overlap = numel(intersect(nn(:,i), nn_(:,i))) / ...
            numel(union(nn(:,i), nn_(:,i))) ;
  assert(overlap > 0.6, 'ANN did not return enough correct nearest neighbors') ;
end

function test_ann_forest(s)
vl_twister('state', 1) ;
numNeighbors = 7 ;
maxComparisons = numNeighbors * 25 ;
numTrees = 5 ;
tree = vl_kdtreebuild(s.X, 'numTrees', 5) ;
[nn, d2] = vl_kdtreequery(tree, s.X, s.Q, ...
                          'numNeighbors', numNeighbors, ...
                          'maxComparisons', maxComparisons) ;

D2 = vl_alldist2(s.X, s.Q, 'l2') ;
[d2_, nn_] = sort(D2) ;
d2_ = d2_(1:numNeighbors, :) ;
nn_ = nn_(1:numNeighbors, :) ;

for i=1:size(s.Q,2)
  overlap = numel(intersect(nn(:,i), nn_(:,i))) / ...
            numel(union(nn(:,i), nn_(:,i))) ;
  assert(overlap > 0.6, 'ANN did not return enough correct nearest neighbors') ;
end
