classdef vl_test_colsubset < matlab.unittest.TestCase  
  properties
    x = [5 2 3 6 4 7 1 9 8 0] ;
  end
  methods (Test)
    function test_beginning(t)
      t.verifyEqual(1:5, vl_colsubset(1:10, 5, 'beginning')) ;
      t.verifyEqual(1:5, vl_colsubset(1:10, .5, 'beginning')) ;
    end    
    function test_ending(t)
      t.verifyEqual(6:10, vl_colsubset(1:10, 5, 'ending')) ;
      t.verifyEqual(6:10, vl_colsubset(1:10, .5, 'ending')) ;
    end
    function test_largest(t)
      t.verifyEqual([5 6 7 9 8], vl_colsubset(t.x, 5, 'largest')) ;
      t.verifyEqual([5 6 7 9 8], vl_colsubset(t.x, .5, 'largest')) ;
    end
    function test_smallest(t)
      t.verifyEqual([2 3 4 1 0], vl_colsubset(t.x, 5, 'smallest')) ;
      t.verifyEqual([2 3 4 1 0], vl_colsubset(t.x, .5, 'smallest')) ;
    end
    function test_random(t)
      t.verifyEqual(numel(intersect(t.x, vl_colsubset(t.x, 5, 'random'))),5) ;
    end
  end
end