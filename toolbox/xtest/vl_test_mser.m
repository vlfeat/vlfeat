function results = vl_test_mser(varargin)
% VL_TEST_MSER
vl_test_init ;

function s = setup()
s.im = im2uint8(rgb2gray(vl_impattern('roofs1'))) ;

function test_mser(s)
[regions,frames] = vl_mser(s.im) ;
mask = vl_erfill(s.im, regions(1)) ;

