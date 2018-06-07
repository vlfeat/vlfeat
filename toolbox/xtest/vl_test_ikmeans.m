classdef vl_test_ikmeans < matlab.unittest.TestCase
  properties
    data
  end
  
  methods (TestClassSetup)
    function setup(t)      
      rand('state',0) ;
      t.data = uint8(rand(2,1000) * 255) ;
    end
  end
  
  methods (Test)
    function test_basic(t)
      [centers, assign] = vl_ikmeans(t.data,100) ;
      assign_ = vl_ikmeanspush(t.data, centers) ;
      t.verifyEqual(assign,assign_) ;
    end
    function test_elkan(t)
      [centers, assign] = vl_ikmeans(t.data,100,'method','elkan') ;
      assign_ = vl_ikmeanspush(t.data, centers) ;
      t.verifyEqual(assign,assign_) ;
    end
  end
end