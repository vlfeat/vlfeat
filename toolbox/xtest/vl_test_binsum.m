classdef vl_test_binsum < matlab.unittest.TestCase
  methods (Test)    
    function test_three_args(t)
      t.verifyEqual(...
        vl_binsum([0 0], 1, 2), [0 1]) ;
      t.verifyEqual(...
        vl_binsum([1 7], -1, 1), [0 7]) ;
      t.verifyEqual(...
        vl_binsum([1 7], -1, [1 2 2 2 2 2 2 2]), [0 0]) ;
    end
    function test_four_args(t)
      t.verifyEqual(...
        vl_binsum(eye(3), [1 1 1],  [1 2 3],  1), 2*eye(3)) ;
      t.verifyEqual(...
        vl_binsum(eye(3), [1 1 1]', [1 2 3]', 2), 2*eye(3)) ;
      t.verifyEqual(...
        vl_binsum(eye(3), 1, [1 2 3], 1), 2*eye(3)) ;
      t.verifyEqual(...
        vl_binsum(eye(3), 1, [1 2 3]', 2), 2*eye(3)) ;
    end
    function test_3d_one(t)
      Z = zeros(3,3,3) ;
      B = 3*ones(3,1,3) ;
      R = Z ; R(:,3,:) = 17 ;
      t.verifyEqual(...
        vl_binsum(Z, 17, B, 2), R) ;
    end
    function test_3d_two(t)
      Z = zeros(3,3,3) ;
      B = 3*ones(3,3,1) ;
      X = zeros(3,3,1) ; X(:,:,1) = 17 ;
      R = Z ; R(:,:,3) = 17 ;
      t.verifyEqual(...
        vl_binsum(Z, X, B, 3), R) ;
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
            vl_binsum(a(eye(3)), a([1 1 1]),  b([1 2 3]),  1), a(2*eye(3))) ;
        end
      end
    end
  end
end