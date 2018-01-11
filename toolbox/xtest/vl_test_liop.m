classdef vl_test_liop < matlab.unittest.TestCase
  properties
    blob
    patch
  end
  
  methods (TestClassSetup)
    function setup(t)
      randn('state',0) ;
      t.patch = randn(65,'single') ;
      xr = -32:32 ;
      [x,y] = meshgrid(xr) ;
      t.blob = - single(x.^2+y.^2) ;
    end
  end
  
  methods (Test)
    function test_basic(t)
      d = vl_liop(t.patch) ;
      t.verifyTrue(true) ;
    end
    function test_blob(t)
      % with a blob, all local intensity order pattern are equal. In
      % particular, if the blob intensity decreases away from the center,
      % then all local intensities sampled in a neighbourhood of 2 elements
      % are already sorted (see LIOP details).
      d = vl_liop(t.blob, ...
        'IntensityThreshold', 0,  ...
        'NumNeighbours', 2, ...
        'NumSpatialBins', 1) ;
      t.verifyEqual(d, single([1;0])) ;
    end
    function test_neighbours(t)
      for n=2:5
        for p=1:3
          d = vl_liop(t.patch, 'NumNeighbours', n, 'NumSpatialBins', p) ;
          t.verifyEqual(numel(d), p * factorial(n)) ;
        end
      end
    end
    function test_multiple(t)
      x = randn(31,31,3, 'single') ;
      d = vl_liop(x) ;
      for i=1:3
        d_(:,i) = vl_liop(squeeze(x(:,:,i))) ;
      end
      t.verifyEqual(d,d_) ;
    end
  end
end