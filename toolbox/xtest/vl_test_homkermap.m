function results = vl_test_homkermap(varargin)
% VL_TEST_HOMKERMAP
vl_test_init ;

function check_ker(ker, n, window, period)
args = {n, ker, 'window', window} ;
if nargin > 3
  args = {args{:}, 'period', period} ;
end
x = [-1 -.5 0 .5 1] ;
y = linspace(0,2,100) ;
for conv = {@single, @double}
  x = feval(conv{1}, x) ;
  y = feval(conv{1}, y) ;
  sx = sign(x) ;
  sy = sign(y) ;
  psix = vl_homkermap(x, args{:}) ;
  psiy = vl_homkermap(y, args{:}) ;
  k = vl_alldist(psix,psiy,'kl2') ;
  k_ = (sx'*sy) .* vl_alldist(sx.*x,sy.*y,ker) ;
  vl_assert_almost_equal(k, k_, 2e-2) ;
end

function test_uniform_kchi2(),     check_ker('kchi2', 3,  'uniform', 15) ;
function test_uniform_kjs(),       check_ker('kjs',   3,  'uniform', 15) ;
function test_uniform_kl1(),       check_ker('kl1',  29,  'uniform', 15) ;
function test_rect_kchi2(),        check_ker('kchi2', 3,  'rectangular', 15) ;
function test_rect_kjs(),          check_ker('kjs',   3,  'rectangular', 15) ;
function test_rect_kl1(),          check_ker('kl1',  29,  'rectangular', 10) ;
function test_auto_uniform_kchi2(),check_ker('kchi2', 3,  'uniform') ;
function test_auto_uniform_kjs(),  check_ker('kjs',   3,  'uniform') ;
function test_auto_uniform_kl1(),  check_ker('kl1',  25,  'uniform') ;
function test_auto_rect_kchi2(),   check_ker('kchi2', 3,  'rectangular') ;
function test_auto_rect_kjs(),     check_ker('kjs',   3,  'rectangular') ;
function test_auto_rect_kl1(),     check_ker('kl1',  25,  'rectangular') ;

function test_gamma()
x = linspace(0,1,20) ;
for gamma = linspace(.2,2,10)
  k = vl_alldist(x, 'kchi2') .* (x'*x + 1e-12).^((gamma-1)/2) ;
  psix = vl_homkermap(x, 3, 'kchi2', 'gamma', gamma) ;
  assert(norm(k - psix'*psix) < 1e-2) ;
end

function test_negative()
x = linspace(-1,1,20) ;
k = vl_alldist(abs(x), 'kchi2') .* (sign(x)'*sign(x)) ;
psix = vl_homkermap(x, 3, 'kchi2') ;
assert(norm(k - psix'*psix) < 1e-2) ;
