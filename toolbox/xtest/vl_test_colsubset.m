function results = vl_test_colsubset(varargin)
% VL_TEST_COLSUBSET
vl_test_init ;

function s = setup()
s.x = [5 2 3 6 4 7 1 9 8 0] ;

function test_beginning(s)
vl_assert_equal(1:5, vl_colsubset(1:10, 5, 'beginning')) ;
vl_assert_equal(1:5, vl_colsubset(1:10, .5, 'beginning')) ;

function test_ending(s)
vl_assert_equal(6:10, vl_colsubset(1:10, 5, 'ending')) ;
vl_assert_equal(6:10, vl_colsubset(1:10, .5, 'ending')) ;

function test_largest(s)
vl_assert_equal([5 6 7 9 8], vl_colsubset(s.x, 5, 'largest')) ;
vl_assert_equal([5 6 7 9 8], vl_colsubset(s.x, .5, 'largest')) ;

function test_smallest(s)
vl_assert_equal([2 3 4 1 0], vl_colsubset(s.x, 5, 'smallest')) ;
vl_assert_equal([2 3 4 1 0], vl_colsubset(s.x, .5, 'smallest')) ;

function test_random(s)
assert(numel(intersect(s.x, vl_colsubset(s.x, 5, 'random'))) == 5) ;
