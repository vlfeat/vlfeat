function results = vl_test_homkermap(varargin)
% VL_TEST_HOMKERMAP

% AUTORIGHTS

vl_test_init ;

function test_basic()
n = 3 ;
L = .4 ;

x = .5 ;
y = linspace(0,2,100) ;

psix = vl_homkermap(x, n, L, 'kchi2') ;
psiy = vl_homkermap(y, n, L, 'kchi2') ;

k = vl_alldist(psix,psiy,'kl2') ;
k_ = vl_alldist(x,y,'kchi2') ;

vl_assert_almost_equal(k, k_, 2e-2) ;
