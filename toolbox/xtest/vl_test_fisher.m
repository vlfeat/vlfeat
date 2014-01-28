function results = vl_test_fisher(varargin)
% VL_TEST_FISHER
vl_test_init ;

function s =  setup()
randn('state',0) ;
dimension = 5 ;
numData = 21 ;
numComponents = 3 ;
s.x = randn(dimension,numData) ;
s.mu = randn(dimension,numComponents) ;
s.sigma2 = ones(dimension,numComponents) ;
s.prior = ones(1,numComponents) ;
s.prior = s.prior / sum(s.prior) ;

function test_basic(s)
phi_ = simple_fisher(s.x, s.mu, s.sigma2, s.prior) ;
phi = vl_fisher(s.x, s.mu, s.sigma2, s.prior) ;
vl_assert_almost_equal(phi, phi_, 1e-10) ;

function test_norm(s)
phi_ = simple_fisher(s.x, s.mu, s.sigma2, s.prior) ;
phi_ = phi_ / norm(phi_) ;
phi = vl_fisher(s.x, s.mu, s.sigma2, s.prior, 'normalized') ;
vl_assert_almost_equal(phi, phi_, 1e-10) ;

function test_sqrt(s)
phi_ = simple_fisher(s.x, s.mu, s.sigma2, s.prior) ;
phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
phi = vl_fisher(s.x, s.mu, s.sigma2, s.prior, 'squareroot') ;
vl_assert_almost_equal(phi, phi_, 1e-10) ;

function test_improved(s)
phi_ = simple_fisher(s.x, s.mu, s.sigma2, s.prior) ;
phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
phi_ = phi_ / norm(phi_) ;
phi = vl_fisher(s.x, s.mu, s.sigma2, s.prior, 'improved') ;
vl_assert_almost_equal(phi, phi_, 1e-10) ;

function test_fast(s)
phi_ = simple_fisher(s.x, s.mu, s.sigma2, s.prior, true) ;
phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
phi_ = phi_ / norm(phi_) ;
phi = vl_fisher(s.x, s.mu, s.sigma2, s.prior, 'improved', 'fast') ;
vl_assert_almost_equal(phi, phi_, 1e-10) ;

function enc = simple_fisher(x, mu, sigma2, pri, fast)
if nargin < 5, fast = false ; end
sigma = sqrt(sigma2) ;
for k = 1:size(mu,2)
  delta{k} = bsxfun(@times, bsxfun(@minus, x, mu(:,k)), 1./sigma(:,k)) ;
  q(k,:) = log(pri(k)) - 0.5 * sum(log(sigma2(:,k))) - 0.5 * sum(delta{k}.^2,1) ;
end
q = exp(bsxfun(@minus, q, max(q,[],1))) ;
q = bsxfun(@times, q, 1 ./ sum(q,1)) ;
n = size(x,2) ;
if fast
  [~,i] = max(q) ;
  q = zeros(size(q)) ;
  q(sub2ind(size(q),i,1:n)) = 1 ;
end
for k = 1:size(mu,2)
  u{k} = delta{k} * q(k,:)' / n / sqrt(pri(k)) ;
  v{k} = (delta{k}.^2 - 1) * q(k,:)' / n / sqrt(2*pri(k)) ;
end
enc = cat(1, u{:}, v{:}) ;
