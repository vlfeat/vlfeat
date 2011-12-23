function results = vl_test_imwbackward(varargin)
% VL_TEST_IMWBACKWARD
vl_test_init ;

function s = setup()
s.I = im2double(imread(fullfile(vl_root,'data','spots.jpg'))) ;

function test_identity(s)
xr = 1:size(s.I,2) ;
yr = 1:size(s.I,1) ;
[x,y] = meshgrid(xr,yr) ;
vl_assert_almost_equal(s.I, vl_imwbackward(xr,yr,s.I,x,y)) ;

function test_invalid_args(s)
xr = 1:size(s.I,2) ;
yr = 1:size(s.I,1) ;
[x,y] = meshgrid(xr,yr) ;
vl_assert_exception(@() vl_imwbackward(xr,yr,single(s.I),x,y), 'vl:invalidArgument') ;
