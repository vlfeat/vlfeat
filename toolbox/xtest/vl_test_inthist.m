classdef vl_test_inthist < matlab.unittest.TestCase
  properties
    labels
  end 
  
  methods (TestClassSetup)
    function setup(t)      
      rand('state',0) ;
      t.labels = uint32(8*rand(123, 76, 3)) ;
    end
  end
  
  methods (Test)
    function test_basic(t)
      l = 10 ;
      hist = vl_inthist(t.labels, 'numlabels', l) ;
      hist_ = t.inthist_slow(t.labels, l) ;
      t.verifyEqual(double(hist),hist_) ;
    end
    function test_sample(t)
      rand('state',0) ;
      boxes = 10 * rand(4,20) + .5 ;
      boxes(3:4,:) = boxes(3:4,:) + boxes(1:2,:) ;
      boxes = min(boxes, 10) ;
      boxes = uint32(boxes) ;
      inthist = vl_inthist(t.labels) ;
      hist = vl_sampleinthist(inthist, boxes) ;
    end
  end
  
  methods (Static)
    function hist = inthist_slow(labels, numLabels)
      m = size(labels,1) ;
      n = size(labels,2) ;
      l = numLabels ;
      b = zeros(m*n,l) ;
      b = vl_binsum(b, 1, reshape(labels,m*n,[]), 2) ;
      b = reshape(b,m,n,l) ;
      for k=1:l
        hist(:,:,k) = cumsum(cumsum(b(:,:,k)')') ;
      end
    end
  end
end