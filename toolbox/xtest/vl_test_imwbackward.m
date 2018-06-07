classdef vl_test_imwbackward < matlab.unittest.TestCase
  properties
    I
  end
    
  methods (TestClassSetup)
    function setup(t)
      t.I = im2double(imread(fullfile(vl_root,'data','spots.jpg'))) ;
    end
  end
  
  methods (Test)
    function test_identity(t)
      xr = 1:size(t.I,2) ;
      yr = 1:size(t.I,1) ;
      [x,y] = meshgrid(xr,yr) ;
      t.verifyEqual(t.I, vl_imwbackward(xr,yr,t.I,x,y)) ;
    end
    function test_invalid_args(t)
      xr = 1:size(t.I,2) ;
      yr = 1:size(t.I,1) ;
      [x,y] = meshgrid(xr,yr) ;
      t.verifyError(@() vl_imwbackward(xr,yr,single(t.I),x,y), 'vl:invalidArgument') ;
    end
  end
end