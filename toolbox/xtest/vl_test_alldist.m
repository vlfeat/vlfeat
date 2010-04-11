function results = vl_test_alldist(varargin)
% VL_TEST_ALLDIST
vl_test_init ;

function s = setup()
vl_twister('state', 0) ;
s.X = 3.1 * vl_twister(10,10) ;
s.Y = 4.7 * vl_twister(10,7) ;

function test_null_args(s)
vl_assert_equal(...
  vl_alldist(zeros(15,12), zeros(15,0), 'kl2'), ...
  zeros(12,0)) ;

vl_assert_equal(...
  vl_alldist(zeros(15,0), zeros(15,0), 'kl2'), ...
  zeros(0,0)) ;

vl_assert_equal(...
  vl_alldist(zeros(15,0), zeros(15,12), 'kl2'), ...
  zeros(0,12)) ;

vl_assert_equal(...
  vl_alldist(zeros(0,15), zeros(0,12), 'kl2'), ...
  zeros(15,12)) ;

function test_self(s)
vl_assert_almost_equal(...
  vl_alldist(s.X, 'kl2'), ...
  makedist(@(x,y) x*y, s.X, s.X), ...
  1e-6) ;

function test_distances(s)
dists = {'chi2', 'l2', 'l1', 'hell', 'js', ...
         'kchi2', 'kl2', 'kl1', 'khell', 'kjs'} ;
distsEquiv = { ...
  @(x,y) (x-y)^2 / (x + y), ...
  @(x,y) (x-y)^2, ...
  @(x,y) abs(x-y), ...
  @(x,y) (sqrt(x) - sqrt(y))^2, ...
  @(x,y) x - x .* log2(1 + y/x) + y - y .* log2(1 + x/y), ...
  @(x,y) 2 * (x*y) / (x + y), ...
  @(x,y) x*y, ...
  @(x,y) min(x,y), ...
  @(x,y) sqrt(x.*y), ...
  @(x,y) .5 * (x .* log2(1 + y/x) + y .* log2(1 + x/y))} ;
types = {'single', 'double'} ;

for simd = [0 1]
  for d = 1:length(dists)
    for t = 1:length(types)
      vl_simdctrl(simd) ;
      X = feval(str2func(types{t}), s.X) ;
      Y = feval(str2func(types{t}), s.Y) ;
      vl_assert_almost_equal(...
        vl_alldist(X,Y,dists{d}), ...
        makedist(distsEquiv{d},X,Y), ...
        1e-4, ...
        'alldist failed for dist=%s type=%s simd=%d', ...
        dists{d}, ...
        types{t}, ...
        simd) ;
    end
  end
end

function test_distance_kernel_pairs(s)
dists = {'chi2', 'l2', 'l1', 'hell', 'js'} ;
for d = 1:length(dists)
  dist = char(dists{d}) ;
  X = s.X ;
  Y = s.Y ;
  ker = ['k' dist] ;
  kxx = vl_alldist(X,X,ker) ;
  kyy = vl_alldist(Y,Y,ker) ;
  kxy = vl_alldist(X,Y,ker) ;
  kxx = repmat(diag(kxx), 1, size(s.Y,2)) ;
  kyy = repmat(diag(kyy), 1, size(s.X,1))' ;
  d2  = vl_alldist(X,Y,dist) ;
  vl_assert_almost_equal(d2, kxx + kyy - 2 * kxy, '1e-6') ;
end

function D = makedist(cmp,X,Y)
[d,m] = size(X) ;
[d,n] = size(Y) ;
D = zeros(m,n) ;
for i = 1:m
  for j = 1:n
    acc = 0 ;
    for k = 1:d
      acc = acc + cmp(X(k,i),Y(k,j)) ;
    end
    D(i,j) = acc ;
  end
end
conv = str2func(class(X)) ;
D = conv(D) ;
