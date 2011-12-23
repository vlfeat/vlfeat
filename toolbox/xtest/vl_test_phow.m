function results = vl_test_phow(varargin)
% VL_TEST_PHOPW
vl_test_init ;

function s = setup()
s.I = im2double(imread(fullfile(vl_root,'data','spots.jpg'))) ;
s.I = single(s.I) ;

function test_gray(s)
[f,d] = vl_phow(s.I, 'color', 'gray') ;
assert(size(d,1) == 128) ;

function test_rgb(s)
[f,d] = vl_phow(s.I, 'color', 'rgb') ;
assert(size(d,1) == 128*3) ;

function test_hsv(s)
[f,d] = vl_phow(s.I, 'color', 'hsv') ;
assert(size(d,1) == 128*3) ;

function test_opponent(s)
[f,d] = vl_phow(s.I, 'color', 'opponent') ;
assert(size(d,1) == 128*3) ;
