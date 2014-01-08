function results = vl_test_lbp(varargin)
% VL_TEST_TWISTER
vl_test_init ;

function test_unfiorm_lbps(s)

% enumerate the 56 uniform lbps
q = 0  ;
for i=0:7
  for j=1:7
    I = zeros(3) ;
    p = mod(s.pixels - i + 8, 8) + 1 ;
    I(p <= j) = 1 ;
    f = vl_lbp(single(I), 3) ;
    q = q + 1 ;
    vl_assert_equal(find(f), q) ;
  end
end

% constant lbps
I = [1 1 1 ; 1 0 1 ; 1 1 1] ;
f = vl_lbp(single(I), 3) ;
vl_assert_equal(find(f), 57) ;

I = [1 1 1 ; 1 1 1 ; 1 1 1] ;
f = vl_lbp(single(I), 3) ;
vl_assert_equal(find(f), 57) ;

% other lbps
I = [1 0 1 ; 0 0 0 ; 1 0 1] ;
f = vl_lbp(single(I), 3) ;
vl_assert_equal(find(f), 58) ;

function test_fliplr(s)
randn('state',0) ;
I = randn(256,256,1,'single') ;
f = vl_lbp(fliplr(I), 8) ;
f_ = vl_lbpfliplr(vl_lbp(I, 8)) ;
vl_assert_almost_equal(f,f_,1e-3) ;

function s = setup()
s.pixels = [5 6 7 ;
            4 NaN 0 ;
            3 2 1] ;
