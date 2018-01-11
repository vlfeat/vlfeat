classdef vl_test_mser < matlab.unittest.TestCase
  properties
    im
  end
  
  methods (TestClassSetup)
    function ssetup(t)
     t.im = im2uint8(rgb2gray(vl_impattern('roofs1'))) ;
    end
  end
   
  methods (Test)
    function test_mser(t)
      [regions,frames] = vl_mser(t.im) ;
      mask = vl_erfill(t.im, regions(1)) ;
      t.verifyTrue(true) ;
    end
  end
end


