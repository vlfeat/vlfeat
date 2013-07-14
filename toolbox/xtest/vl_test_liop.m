function results = vl_test_liop(varargin)
% VL_TEST_SIFT
vl_test_init ;

function s = setup()
randn('state',0) ;
s.patch = randn(65,'single') ;

function test_basic(s)
d = vl_liop(s.patch)
tt=load;tt.d-d
