function results = vl_test_homkermap(varargin)
% VL_TEST_HOMKERMAP
vl_test_init ;

function test_basic_kchi2()
n = 3 ;
L = .4 ;
x = .5 ;
y = linspace(0,2,100) ;

for conv = {@single, @double}
  x = feval(conv{1}, x) ;
  y = feval(conv{1}, y) ;
  psix = vl_homkermap(x, n, L, 'kchi2') ;
  psiy = vl_homkermap(y, n, L, 'kchi2') ;
  k = vl_alldist(psix,psiy,'kl2') ;
  k_ = vl_alldist(x,y,'kchi2') ;
  vl_assert_almost_equal(k, k_, 2e-2) ;
end

function test_basic_kl1()
n = 15 ;
L = .4 ;
x = .5 ;
y = linspace(0,2,100) ;

for conv = {@single, @double}
  x = feval(conv{1}, x) ;
  y = feval(conv{1}, y) ;
  psix = vl_homkermap(x, n, L, 'kl1') ;
  psiy = vl_homkermap(y, n, L, 'kl1') ;
  k = vl_alldist(psix,psiy,'kl2') ;
  k_ = vl_alldist(x,y,'kl1') ;
  vl_assert_almost_equal(k, k_, 2e-1) ;
end

function test_basic_kjs()
n = 15 ;
L = .4 ;
x = .5 ;
y = linspace(0,2,100) ;

for conv = {@single, @double}
  x = feval(conv{1}, x) ;
  y = feval(conv{1}, y) ;
  psix = vl_homkermap(x, n, L, 'kjs') ;
  psiy = vl_homkermap(y, n, L, 'kjs') ;
  k = vl_alldist(psix,psiy,'kl2') ;
  k_ = vl_alldist(x,y,'kjs') ;
  vl_assert_almost_equal(k, k_, 2e-2) ;
end
