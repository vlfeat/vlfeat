function results = vl_test_vlad(varargin)
% VL_TEST_VLAD
vl_test_init ;

function s =  setup()
randn('state',0) ;
s.x = randn(128,256) ;
s.mu = randn(128,16) ;
assignments = rand(16, 256) ;
s.assignments = bsxfun(@times, assignments, 1 ./ sum(assignments,1)) ;

function test_basic (s)
x = [1, 2, 3] ;
mu = [0, 0, 0] ;
assignments = eye(3) ;
phi = vl_vlad(x, mu, assignments, 'unnormalized') ;
vl_assert_equal(phi, [1 2 3]') ;

mu = [0, 1, 2] ;
phi = vl_vlad(x, mu, assignments, 'unnormalized') ;
vl_assert_equal(phi, [1 1 1]') ;
phi = vl_vlad([x x], mu, [assignments assignments], 'unnormalized') ;
vl_assert_equal(phi, [2 2 2]') ;

function test_rand (s)
phi_ = simple_vlad(s.x, s.mu, s.assignments) ;
phi = vl_vlad(s.x, s.mu, s.assignments, 'unnormalized') ;
vl_assert_equal(phi, phi_) ;

function test_norm (s)
phi_ = simple_vlad(s.x, s.mu, s.assignments) ;
phi_ = phi_ / norm(phi_) ;
phi = vl_vlad(s.x, s.mu, s.assignments) ;
vl_assert_almost_equal(phi, phi_, 1e-4) ;

function test_sqrt (s)
phi_ = simple_vlad(s.x, s.mu, s.assignments) ;
phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
phi_ = phi_ / norm(phi_) ;
phi = vl_vlad(s.x, s.mu, s.assignments, 'squareroot') ;
vl_assert_almost_equal(phi, phi_, 1e-4) ;

function test_individual (s)
phi_ = simple_vlad(s.x, s.mu, s.assignments) ;
phi_ = reshape(phi_, size(s.x,1), []) ;
phi_ = bsxfun(@times, phi_, 1 ./ sqrt(sum(phi_.^2))) ;
phi_ = phi_(:) ;
phi = vl_vlad(s.x, s.mu, s.assignments, 'unnormalized', 'normalizecomponents') ;
vl_assert_almost_equal(phi, phi_, 1e-4) ;

function test_mass (s)
phi_ = simple_vlad(s.x, s.mu, s.assignments) ;
phi_ = reshape(phi_, size(s.x,1), []) ;
phi_ = bsxfun(@times, phi_, 1 ./ sum(s.assignments,2)') ;
phi_ = phi_(:) ;
phi = vl_vlad(s.x, s.mu, s.assignments, 'unnormalized', 'normalizemass') ;
vl_assert_almost_equal(phi, phi_, 1e-4) ;

function enc = simple_vlad(x, mu, assign)
for i = 1:size(assign,1)
  enc{i} = x * assign(i,:)' - sum(assign(i,:)) * mu(:,i) ;
end
enc = cat(1, enc{:}) ;
