classdef vl_test_binsearch < matlab.unittest.TestCase
  methods (Test)    
    function test_inf_bins(t)
      x = [-inf -1 0 1 +inf] ;
      t.verifyEqual(vl_binsearch([],          x), [0 0 0 0 0]) ;
      t.verifyEqual(vl_binsearch([-inf 0],    x), [1 1 2 2 2]) ;
      t.verifyEqual(vl_binsearch([-inf],      x), [1 1 1 1 1]) ;
      t.verifyEqual(vl_binsearch([-inf +inf], x), [1 1 1 1 2]) ;
    end
    function test_empty(t)
      t.verifyEqual(vl_binsearch([], []), []) ;
    end
    function test_bnd(t)
      t.verifyEqual(vl_binsearch([], [1]),    [0]) ;
      t.verifyEqual(vl_binsearch([], [-inf]), [0]) ;
      t.verifyEqual(vl_binsearch([], [+inf]), [0]) ;
      t.verifyEqual(vl_binsearch([1], [.9]),   [0]) ;
      t.verifyEqual(vl_binsearch([1], [1]),    [1]) ;
      t.verifyEqual(vl_binsearch([1], [-inf]), [0]) ;
      t.verifyEqual(vl_binsearch([1], [+inf]), [1]) ;
    end
    function test_basic(t)
      t.verifyEqual(vl_binsearch(-10:10, -10:10), 1:21) ;
      t.verifyEqual(vl_binsearch(-10:10, -11:10), 0:21) ;
      t.verifyEqual(vl_binsearch(-10:10, [-inf, -11:10, +inf]), [0 0:21 21]) ;
    end
    function test_frac(t)
      t.verifyEqual(vl_binsearch(1:10, 1:.5:10), floor(1:.5:10))
      t.verifyEqual(vl_binsearch(1:10, fliplr(1:.5:10)), ...
        fliplr(floor(1:.5:10))) ;
    end
    function test_array(t)
      a = reshape(1:100,10,10) ;
      b = reshape(1:.5:100.5, 2, []) ;
      c = floor(b) ;
      t.verifyEqual(vl_binsearch(a,b), c) ;
    end
  end
end