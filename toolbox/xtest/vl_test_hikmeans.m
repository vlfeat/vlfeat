classdef vl_test_hikmeans < matlab.unittest.TestCase
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
      [tree, assign] = vl_hikmeans(t.data,3,100) ;
      assign_ = vl_hikmeanspush(tree, t.data) ;
      t.verifyEqual(assign,assign_) ;
    end
    function test_elkan(t)
      [tree, assign] = vl_hikmeans(t.data,3,100,'method','elkan') ;
      assign_ = vl_hikmeanspush(tree, t.data) ;
      t.verifyEqual(assign,assign_) ;
    end
  end
end