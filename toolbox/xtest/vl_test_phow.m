classdef vl_test_phow < matlab.unittest.TestCase
  properties
    I
  end
  
  methods (TestClassSetup)
    function ssetup(t)
      t.I = im2double(imread(fullfile(vl_root,'data','spots.jpg'))) ;
      t.I = single(t.I) ;
    end
  end
  
  methods (Test)
    function test_gray(t)
      [f,d] = vl_phow(t.I, 'color', 'gray') ;
      t.verifyEqual(size(d,1),128) ;
    end
    function test_rgb(t)
      [f,d] = vl_phow(t.I, 'color', 'rgb') ;
      t.verifyEqual(size(d,1),128*3) ;
    end
    function test_hsv(t)
      [f,d] = vl_phow(t.I, 'color', 'hsv') ;
      t.verifyEqual(size(d,1),128*3) ;
    end
    function test_opponent(t)
      [f,d] = vl_phow(t.I, 'color', 'opponent') ;
      t.verifyEqual(size(d,1),128*3) ;
    end
  end
end

