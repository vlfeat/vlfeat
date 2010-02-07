% VL_TEST_BINSEARCH
function results = vl_test_binsearch(varargin)
vl_test_init ;

function test_empty()
vl_assert_equal(vl_binsearch([], []), []) ;

function test_bnd()
vl_assert_equal(vl_binsearch([], [1]),    [0]) ;
vl_assert_equal(vl_binsearch([], [-inf]), [0]) ;
vl_assert_equal(vl_binsearch([], [+inf]), [0]) ;

vl_assert_equal(vl_binsearch([1], [.9]),   [0]) ;
vl_assert_equal(vl_binsearch([1], [1]),    [1]) ;
vl_assert_equal(vl_binsearch([1], [-inf]), [0]) ;
vl_assert_equal(vl_binsearch([1], [+inf]), [1]) ;

function test_basic()
vl_assert_equal(vl_binsearch(-10:10, -10:10), 1:21) ;
vl_assert_equal(vl_binsearch(-10:10, -11:10), 0:21) ;
vl_assert_equal(vl_binsearch(-10:10, [-inf, -11:10, +inf]), [0 0:21 21]) ;

function test_frac()
vl_assert_equal(vl_binsearch(1:10, 1:.5:10), floor(1:.5:10))
vl_assert_equal(vl_binsearch(1:10, fliplr(1:.5:10)), ...
                fliplr(floor(1:.5:10))) ;

function test_array()
a = reshape(1:100,10,10) ;
b = reshape(1:.5:100.5, 2, []) ;
c = floor(b) ;
vl_assert_equal(vl_binsearch(a,b), c) ;
