function results = vl_test_twister(varargin)
% VL_TEST_TWISTER
vl_test_init ;

function test_illegal_args()
vl_assert_exception(@() vl_twister(-1), 'vl:invalidArgument') ;
vl_assert_exception(@() vl_twister(1, -1), 'vl:invalidArgument') ;
vl_assert_exception(@() vl_twister([1, -1]), 'vl:invalidArgument') ;

function test_seed_by_scalar()
rand('twister',1) ; a = rand ;
vl_twister('state',1) ; b = vl_twister ;
vl_assert_equal(a,b,'seed by scalar + VL_TWISTER()') ;

function test_get_set_state()
rand('twister',1) ; a = rand('twister') ;
vl_twister('state',1) ; b = vl_twister('state') ;
vl_assert_equal(a,b,'read state') ;

a(1) = a(1) + 1 ;
vl_twister('state',a) ; b = vl_twister('state') ;
vl_assert_equal(a,b,'set state') ;

function test_multi_dimensions()
b = rand('twister') ;
rand('twister',b) ;
vl_twister('state',b) ;
a=rand([1 2 3 4 5]) ;
b=vl_twister([1 2 3 4 5]) ;
vl_assert_equal(a,b,'VL_TWISTER([M N P ...])') ;

function test_multi_multi_args()
rand('twister',1) ; a=rand(1, 2, 3, 4, 5) ;
vl_twister('state',1) ; b=vl_twister(1, 2, 3, 4, 5) ;
vl_assert_equal(a,b,'VL_TWISTER(M, N, P, ...)') ;

function test_square()
rand('twister',1) ; a=rand(10) ;
vl_twister('state',1) ; b=vl_twister(10) ;
vl_assert_equal(a,b,'VL_TWISTER(N)') ;

