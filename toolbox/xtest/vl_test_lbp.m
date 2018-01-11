classdef vl_test_lbp < matlab.unittest.TestCase
  properties
    pixels
  end
  
  methods (TestClassSetup)
    function setup(t)
      t.pixels = [5 6 7 ;
        4 NaN 0 ;
        3 2 1] ;      
    end
  end
  
  methods (Test)       
    function test_unfiorm_lbps(t)      
      % enumerate the 56 uniform lbps
      q = 0  ;
      for i=0:7
        for j=1:7
          I = zeros(3) ;
          p = mod(t.pixels - i + 8, 8) + 1 ;
          I(p <= j) = 1 ;
          f = vl_lbp(single(I), 3) ;
          q = q + 1 ;
          t.verifyEqual(find(f), q) ;
        end
      end
      
      % constant lbps
      I = [1 1 1 ; 1 0 1 ; 1 1 1] ;
      f = vl_lbp(single(I), 3) ;
      t.verifyEqual(find(f), 57) ;
      
      I = [1 1 1 ; 1 1 1 ; 1 1 1] ;
      f = vl_lbp(single(I), 3) ;
      t.verifyEqual(find(f), 57) ;
      
      % other lbps
      I = [1 0 1 ; 0 0 0 ; 1 0 1] ;
      f = vl_lbp(single(I), 3) ;
      t.verifyEqual(find(f), 58) ;
    end
    function test_fliplr(t)
      I = randn(256,256,1,'single') ;
      f = vl_lbp(fliplr(I), 8) ;
      f_ = vl_lbpfliplr(vl_lbp(I, 8)) ;
      t.verifyEqual(f,f_,1e-3) ;
    end
  end 
end
