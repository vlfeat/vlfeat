function results = vl_test_hog(varargin)
% VL_TEST_HOG
vl_test_init ;

function s = setup()
s.im = im2single(imread(fullfile(vl_root,'data','a.jpg'))) ;

function test_hog(s)
hog = vl_hog(im) ;
