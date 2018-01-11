classdef vl_test_kdtree < matlab.unittest.TestCase
  properties
    X
    Q
  end
  
  methods (TestClassSetup)
    function setup(t)
      randn('state',0) ;
      t.X = single(randn(10, 1000)) ;
      t.Q = single(randn(10, 10)) ;
    end
  end
  
  methods (Test)
    function test_nearest(t)
      for tmethod = {'median', 'mean'}
        for type = {@single, @double}
          conv = type{1} ;
          tmethod = char(tmethod) ;
          
          X = conv(t.X) ;
          Q = conv(t.Q) ;
          tree = vl_kdtreebuild(X,'ThresholdMethod', tmethod) ;
          [nn, d2] = vl_kdtreequery(tree, X, Q) ;
          
          D2 = vl_alldist2(X, Q, 'l2') ;
          [d2_, nn_] = min(D2) ;
          
          t.verifyEqual(...
            nn,uint32(nn_),...
            sprintf('incorrect nns: type=%s th. method=%s', func2str(conv), tmethod)) ;
          t.verifyEqual(...
            d2,d2_,'AbsTol',cast(1e-5,'like',d2),...
            sprintf('incorrect distances: type=%s th. method=%s', func2str(conv), tmethod)) ;
        end
      end
    end
    function test_nearests(t)
      numNeighbors = 7 ;
      tree = vl_kdtreebuild(t.X) ;
      [nn, d2] = vl_kdtreequery(tree, t.X, t.Q, ...
        'numNeighbors', numNeighbors) ;
      
      D2 = vl_alldist2(t.X, t.Q, 'l2') ;
      [d2_, nn_] = sort(D2) ;
      d2_ = d2_(1:numNeighbors, :) ;
      nn_ = nn_(1:numNeighbors, :) ;
      
      t.verifyEqual(nn,uint32(nn_)) ;
      t.verifyEqual(d2,d2_,'AbsTol',cast(1e-5,'like',d2)) ;
    end
    function test_ann(t)
      vl_twister('state', 1) ;
      numNeighbors = 7 ;
      maxComparisons = numNeighbors * 50 ;
      tree = vl_kdtreebuild(t.X) ;
      [nn, d2] = vl_kdtreequery(tree, t.X, t.Q, ...
        'numNeighbors', numNeighbors, ...
        'maxComparisons', maxComparisons) ;
      
      D2 = vl_alldist2(t.X, t.Q, 'l2') ;
      [d2_, nn_] = sort(D2) ;
      d2_ = d2_(1:numNeighbors, :) ;
      nn_ = nn_(1:numNeighbors, :) ;
      
      for i=1:size(t.Q,2)
        overlap = numel(intersect(nn(:,i), nn_(:,i))) / ...
          numel(union(nn(:,i), nn_(:,i))) ;
        t.verifyTrue(overlap > 0.6, 'ANN did not return enough correct nearest neighbors') ;
      end
    end
    
    function test_ann_forest(t)
      vl_twister('state', 1) ;
      numNeighbors = 7 ;
      maxComparisons = numNeighbors * 25 ;
      numTrees = 5 ;
      tree = vl_kdtreebuild(t.X, 'numTrees', 5) ;
      [nn, d2] = vl_kdtreequery(tree, t.X, t.Q, ...
        'numNeighbors', numNeighbors, ...
        'maxComparisons', maxComparisons) ;
      
      D2 = vl_alldist2(t.X, t.Q, 'l2') ;
      [d2_, nn_] = sort(D2) ;
      d2_ = d2_(1:numNeighbors, :) ;
      nn_ = nn_(1:numNeighbors, :) ;
      
      for i=1:size(t.Q,2)
        overlap = numel(intersect(nn(:,i), nn_(:,i))) / ...
          numel(union(nn(:,i), nn_(:,i))) ;
        t.verifyTrue(overlap > 0.6, 'ANN did not return enough correct nearest neighbors') ;
      end
    end
  end
end