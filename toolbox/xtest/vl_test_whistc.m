function results = vl_test_whistc(varargin)
% VL_TEST_WHISTC
vl_test_init ;

function test_acc()
x = ones(1, 10) ;
e = 1 ;
o = 1:10 ;
vl_assert_equal(vl_whistc(x, o, e), 55) ;

function test_basic()
x = 1:10 ;
e = 1:10 ;
o = ones(1, 10) ;
vl_assert_equal(histc(x, e), vl_whistc(x, o, e)) ;

x = linspace(-1,11,100) ;
o = ones(size(x)) ;
vl_assert_equal(histc(x, e), vl_whistc(x, o, e)) ;

function test_multidim()
x = rand(10, 20, 30) ;
e = linspace(0,1,10) ;
o = ones(size(x)) ;

vl_assert_equal(histc(x, e), vl_whistc(x, o, e)) ;
vl_assert_equal(histc(x, e, 1), vl_whistc(x, o, e, 1)) ;
vl_assert_equal(histc(x, e, 2), vl_whistc(x, o, e, 2)) ;
vl_assert_equal(histc(x, e, 3), vl_whistc(x, o, e, 3)) ;

function test_nan()
x = rand(10, 20, 30) ;
e = linspace(0,1,10) ;
o = ones(size(x)) ;
x(1:7:end) = NaN ;

vl_assert_equal(histc(x, e), vl_whistc(x, o, e)) ;
vl_assert_equal(histc(x, e, 1), vl_whistc(x, o, e, 1)) ;
vl_assert_equal(histc(x, e, 2), vl_whistc(x, o, e, 2)) ;
vl_assert_equal(histc(x, e, 3), vl_whistc(x, o, e, 3)) ;

function test_no_edges()
x = rand(10, 20, 30) ;
o = ones(size(x)) ;
vl_assert_equal(histc(1, []), vl_whistc(1, 1, [])) ;
vl_assert_equal(histc(x, []), vl_whistc(x, o, [])) ;
vl_assert_equal(histc(x, [], 1), vl_whistc(x, o, [], 1)) ;
vl_assert_equal(histc(x, [], 2), vl_whistc(x, o, [], 2)) ;
vl_assert_equal(histc(x, [], 3), vl_whistc(x, o, [], 3)) ;
