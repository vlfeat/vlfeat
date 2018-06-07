classdef vl_test_imintegral < matlab.unittest.TestCase
  properties
    I
    correct
  end
  
  methods (TestClassSetup)
    function setup(t)
      t.I = ones(5,6) ;
      t.correct = [ ...
        1     2     3     4     5     6 ;
        2     4     6     8    10    12 ;
        3     6     9    12    15    18 ;
        4     8    12    16    20    24 ;
        5    10    15    20    25    30 ; ] ;
    end
  end
  
  methods (Test)
    function test_matlab_equivalent(t)
      vl_assert_equal(t.slow_imintegral(t.I), t.correct) ;
    end
    
    function test_basic(t)
      vl_assert_equal(vl_imintegral(t.I), t.correct) ;
    end
    function test_multi_dimensional(t)
      vl_assert_equal(vl_imintegral(repmat(t.I, [1 1 3])), ...
        repmat(t.correct, [1 1 3])) ;
    end
    function test_random(t)
      numTests = 50 ;
      for i = 1:numTests
        I = rand(5) ;
        vl_assert_almost_equal(vl_imintegral(t.I), ...
          t.slow_imintegral(t.I)) ;
      end
    end
    function test_datatypes(t)
      vl_assert_equal(single(vl_imintegral(t.I)), single(t.correct)) ;
      vl_assert_equal(double(vl_imintegral(t.I)), double(t.correct)) ;
      vl_assert_equal(uint32(vl_imintegral(t.I)), uint32(t.correct)) ;
      vl_assert_equal(int32(vl_imintegral(t.I)), int32(t.correct)) ;
      vl_assert_equal(int32(vl_imintegral(-t.I)), -int32(t.correct)) ;
    end
  end
  methods (Static)
    function integral = slow_imintegral(I)
      integral = zeros(size(I));
      for k = 1:size(I,3)
        for r = 1:size(I,1)
          for c = 1:size(I,2)
            integral(r,c,k) = sum(sum(I(1:r,1:c,k)));
          end
        end
      end
    end
  end
end
