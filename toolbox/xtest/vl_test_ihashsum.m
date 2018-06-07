classdef vl_test_ihashsum < matlab.unittest.TestCase
  properties
    data
  end
  
  methods (TestClassSetup)
    function setup(t)
      rand('state',0) ;
      t.data = uint8(round(16*rand(2,100))) ;
      sel = find(all(t.data==0)) ;
      t.data(1,sel)=1 ;
    end
  end
  
  methods (Test)
    function test_hash(t)
      D = size(t.data,1) ;
      K = 5 ;
      h = zeros(1,K,'uint32') ;
      id = zeros(D,K,'uint8');
      next = zeros(1,K,'uint32') ;
      [h,id,next] = vl_ihashsum(h,id,next,K,t.data) ;      
      sel = vl_ihashfind(id,next,K,t.data) ;
      count = double(h(sel)) ;      
      [drop,i,j] = unique(t.data','rows') ;
      for k=1:size(t.data,2)
        count_(k) = sum(j == j(k)) ;
      end
      t.verifyEqual(count,count_) ;
    end
  end
end

