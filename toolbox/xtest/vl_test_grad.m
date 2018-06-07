classdef vl_test_grad < matlab.unittest.TestCase
  properties
    I
    I_small
  end
  
  methods (TestClassSetup)
    function setup(t)
      t.I = rand(150,253) ;
      t.I_small = rand(2,2) ;
    end
  end
  
  methods (Test)
    function test_equiv(t)
      t.verifyEqual(gradient(t.I), vl_grad(t.I)) ;
    end
    function test_equiv_small(t)
      t.verifyEqual(gradient(t.I_small), vl_grad(t.I_small)) ;
    end
    function test_equiv_forward(t)
      Ix = diff(t.I,2,1) ;
      Iy = diff(t.I,2,1) ;
      t.verifyEqual(gradient(t.I_small), vl_grad(t.I_small)) ;
    end
  end
end