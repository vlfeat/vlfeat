classdef vl_test_whistc < matlab.unittest.TestCase
  methods (Test)
    function test_acc(t)
      x = ones(1, 10) ;
      e = 1 ;
      o = 1:10 ;
      t.verifyEqual(vl_whistc(x, o, e), 55) ;
    end
    function test_basic(t)
      x = 1:10 ;
      e = 1:10 ;
      o = ones(1, 10) ;
      t.verifyEqual(histc(x, e), vl_whistc(x, o, e)) ;
      
      x = linspace(-1,11,100) ;
      o = ones(size(x)) ;
      t.verifyEqual(histc(x, e), vl_whistc(x, o, e)) ;
    end
    function test_multidim(t)
      x = rand(10, 20, 30) ;
      e = linspace(0,1,10) ;
      o = ones(size(x)) ;
      
      t.verifyEqual(histc(x, e), vl_whistc(x, o, e)) ;
      t.verifyEqual(histc(x, e, 1), vl_whistc(x, o, e, 1)) ;
      t.verifyEqual(histc(x, e, 2), vl_whistc(x, o, e, 2)) ;
      t.verifyEqual(histc(x, e, 3), vl_whistc(x, o, e, 3)) ;
    end
    function test_nan(t)
      x = rand(10, 20, 30) ;
      e = linspace(0,1,10) ;
      o = ones(size(x)) ;
      x(1:7:end) = NaN ;
      
      t.verifyEqual(histc(x, e), vl_whistc(x, o, e)) ;
      t.verifyEqual(histc(x, e, 1), vl_whistc(x, o, e, 1)) ;
      t.verifyEqual(histc(x, e, 2), vl_whistc(x, o, e, 2)) ;
      t.verifyEqual(histc(x, e, 3), vl_whistc(x, o, e, 3)) ;
    end
    function test_no_edges(t)
      x = rand(10, 20, 30) ;
      o = ones(size(x)) ;
      t.verifyEqual(histc(1, []), vl_whistc(1, 1, [])) ;
      t.verifyEqual(histc(x, []), vl_whistc(x, o, [])) ;
      t.verifyEqual(histc(x, [], 1), vl_whistc(x, o, [], 1)) ;
      t.verifyEqual(histc(x, [], 2), vl_whistc(x, o, [], 2)) ;
      t.verifyEqual(histc(x, [], 3), vl_whistc(x, o, [], 3)) ;
    end
  end
end






