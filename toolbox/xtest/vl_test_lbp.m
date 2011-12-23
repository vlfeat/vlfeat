function results = vl_test_lbp(varargin)
% VL_TEST_TWISTER
vl_test_init ;

function test_one_on()
I = {} ;
I{1} = [0 0 0 ; 0 0 1 ; 0 0 0] ;
I{2} = [0 0 0 ; 0 0 0 ; 0 0 1] ;
I{3} = [0 0 0 ; 0 0 0 ; 0 1 0] ;
I{4} = [0 0 0 ; 0 0 0 ; 1 0 0] ;
I{5} = [0 0 0 ; 1 0 0 ; 0 0 0] ;
I{6} = [1 0 0 ; 0 0 0 ; 0 0 0] ;
I{7} = [0 1 0 ; 0 0 0 ; 0 0 0] ;
I{8} = [0 0 1 ; 0 0 0 ; 0 0 0] ;

for j=0:7
  h = vl_lbp(single(I{j+1}), 3) ;
  h = find(squeeze(h)) ;
  vl_assert_equal(h, j * 7 + 1) ;
end

function test_two_on()
I = {} ;
I{1} = [0 0 0 ; 0 0 1 ; 0 0 1] ;
I{2} = [0 0 0 ; 0 0 0 ; 0 1 1] ;
I{3} = [0 0 0 ; 0 0 0 ; 1 1 0] ;
I{4} = [0 0 0 ; 1 0 0 ; 1 0 0] ;
I{5} = [1 0 0 ; 1 0 0 ; 0 0 0] ;
I{6} = [1 1 0 ; 0 0 0 ; 0 0 0] ;
I{7} = [0 1 1 ; 0 0 0 ; 0 0 0] ;
I{8} = [0 0 1 ; 0 0 1 ; 0 0 0] ;

for j=0:7
  h = vl_lbp(single(I{j+1}), 3) ;
  h = find(squeeze(h)) ;
  vl_assert_equal(h, j * 7 + 2) ;
end

function test_fliplr()
randn('state',0) ;
I = randn(256,256,1,'single') ;
f = vl_lbp(fliplr(I), 8) ;
f_ = vl_lbpfliplr(vl_lbp(I, 8)) ;
vl_assert_almost_equal(f,f_,1e-3) ;
