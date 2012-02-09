function results = vl_test_grad(varargin)
% VL_TEST_GRAD
vl_test_init ;

function s = setup()
s.I = rand(150,253) ;
s.I_small = rand(2,2) ;

function test_equiv(s)
vl_assert_equal(gradient(s.I), vl_grad(s.I)) ;

function test_equiv_small(s)
vl_assert_equal(gradient(s.I_small), vl_grad(s.I_small)) ;

function test_equiv_forward(s)
Ix = diff(s.I,2,1) ;
Iy = diff(s.I,2,1) ;

vl_assert_equal(gradient(s.I_small), vl_grad(s.I_small)) ;
