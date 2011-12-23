function results = vl_test_slic(varargin)
% VL_TEST_SLIC
vl_test_init ;

function s = setup()
s.im = im2single(imread(fullfile(vl_root,'data','a.jpg'))) ;

function test_slic(s)
segmentation = vl_slic(s.im, 10, 0.1, 'verbose') ;
