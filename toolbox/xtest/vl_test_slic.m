classdef vl_test_slic < matlab.unittest.TestCase
  properties
    im
  end
  
  methods (TestClassSetup)
    function ssetup(t)
      t.im = im2single(vl_impattern('roofs1')) ;      
    end
  end
  
  methods (Test)
    function test_slic(t)
      segmentation = vl_slic(t.im, 10, 0.1) ;
      t.verifyTrue(true) ;
    end
  end
end
