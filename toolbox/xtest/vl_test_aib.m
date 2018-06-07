classdef vl_test_aib < matlab.unittest.TestCase
  properties
    s = [] ;
  end
  
  methods (TestClassSetup)
    function setup(t)
      s = [] ;
    end
  end
  
  methods (Test)
    function test_basic(t)
      Pcx = [.3 .3 0   0
        0   0   .2 .2] ;
      
      % This results in the AIB tree
      %
      %  1 - \
      %       5 - \
      %  2 - /     \
      %             - 7
      %  3 - \     /
      %       6 - /
      %  4 - /
      %
      % coded by the map [5 5 6 6 7 1] (1 denotes the root).
      
      [parents,cost] = vl_aib(Pcx) ;
      t.verifyEqual(parents, uint32([5 5 6 6 7 7 1])) ;
      t.verifyEqual(vl_test_aib.mi(Pcx)*[1 1 1], cost(1:3),'absTol',1e-3) ;
      
      [cut,map,short] = vl_aibcut(parents,2) ;
      t.verifyEqual(cut, [5 6]) ;
      t.verifyEqual(map, [1 1 2 2 1 2 0]) ;
      t.verifyEqual(short, [5 5 6 6 5 6 7]) ;
    end
    
    function test_cluster_null(t)
      Pcx = [.5 .5   0   0
        0   0   0   0] ;
      
      % This results in the AIB tree
      %
      %  1 - \
      %       5
      %  2 - /
      %
      %  3 x
      %
      %  4 x
      %
      % If ClusterNull is specified, the values 3 and 4
      % which have zero probability are merged first
      %
      %  1 ----------\
      %               7
      %  2 ----- \   /
      %           6-/
      %  3 -\    /
      %      5 -/
      %  4 -/
      
      parents1 = vl_aib(Pcx) ;
      parents2 = vl_aib(Pcx,'ClusterNull') ;
      t.verifyEqual(parents1, uint32([5 5 0 0 1 0 0])) ;
      t.verifyEqual(parents2(3), parents2(4)) ;
    end
  end
  
  methods (Static)
    function x = mi(P)
      % mutual information
      P1 = sum(P,1) ;
      P2 = sum(P,2) ;
      x = sum(sum(P .* log(max(P,1e-10) ./ (P2*P1)))) ;
    end
  end
end