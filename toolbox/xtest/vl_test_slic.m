function results = vl_test_slic(varargin)
% VL_TEST_SLIC
vl_test_init ;

function s = setup()
s.im = im2single(vl_impattern('roofs1')) ;

function test_slic(s)
segmentation = vl_slic(s.im, 10, 0.1) ;
