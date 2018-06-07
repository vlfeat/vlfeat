classdef vl_test_cummax < matlab.unittest.TestCase  
  methods (Test)
    function test_basic(t)
      t.verifyEqual(...
        vl_cummax(1), 1) ;
      t.verifyEqual(...
        vl_cummax([1 2 3 4], 2), [1 2 3 4]) ;
    end
    function test_multidim(t)
      a = [1 2 3 4 3 2 1] ;
      b = [1 2 3 4 4 4 4] ;
      for k=1:6
        dims = ones(1,6) ;
        dims(k) = numel(a) ;
        a = reshape(a, dims) ;
        b = reshape(b, dims) ;
        t.verifyEqual(...
          vl_cummax(a, k), b) ;
      end
    end
    function test_storage_classes(t)
      types = {@double, @single, ...
        @int32, @uint32, ...
        @int16, @uint16, ...
        @int8, @uint8} ;
      if vl_matlabversion() > 71000
        types = horzcat(types, {@int64, @uint64}) ;
      end
      for a = types
        a = a{1} ;
        for b = types
          b = b{1} ;
          t.verifyEqual(...
            vl_cummax(a(eye(3))), a(toeplitz([1 1 1], [1 0 0 ]))) ;
        end
      end
    end
  end
end