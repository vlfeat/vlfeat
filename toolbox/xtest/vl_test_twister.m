classdef vl_test_twister < matlab.unittest.TestCase
  methods (Test)
    function test_illegal_args(t)
      t.verifyError(@() vl_twister(-1), 'vl:invalidArgument') ;
      t.verifyError(@() vl_twister(1, -1), 'vl:invalidArgument') ;
      t.verifyError(@() vl_twister([1, -1]), 'vl:invalidArgument') ;
    end
    function test_seed_by_scalar(t)
      rand('twister',1) ; a = rand ;
      vl_twister('state',1) ; b = vl_twister ;
      t.verifyEqual(a,b,'seed by scalar + VL_TWISTER()') ;
    end
    function test_get_set_state(t)
      rand('twister',1) ; a = rand('twister') ;
      vl_twister('state',1) ; b = vl_twister('state') ;
      t.verifyEqual(a,b,'read state') ;
      
      a(1) = a(1) + 1 ;
      vl_twister('state',a) ; b = vl_twister('state') ;
      t.verifyEqual(a,b,'set state') ;
    end
    function test_multi_dimensions(t)
      b = rand('twister') ;
      rand('twister',b) ;
      vl_twister('state',b) ;
      a=rand([1 2 3 4 5]) ;
      b=vl_twister([1 2 3 4 5]) ;
      t.verifyEqual(a,b,'VL_TWISTER([M N P ...])') ;
    end
    function test_multi_multi_args(t)
      rand('twister',1) ; a=rand(1, 2, 3, 4, 5) ;
      vl_twister('state',1) ; b=vl_twister(1, 2, 3, 4, 5) ;
      t.verifyEqual(a,b,'VL_TWISTER(M, N, P, ...)') ;
    end
    function test_square(t)
      rand('twister',1) ; a=rand(10) ;
      vl_twister('state',1) ; b=vl_twister(10) ;
      t.verifyEqual(a,b,'VL_TWISTER(N)') ;
    end
  end
end




