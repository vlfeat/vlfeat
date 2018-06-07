classdef vl_test_fisher < matlab.unittest.TestCase
  properties
    x
    mu
    sigma2
    prior
  end
  
  methods (TestClassSetup)
    function setup(t)
      randn('state',0) ;
      dimension = 5 ;
      numData = 21 ;
      numComponents = 3 ;
      t.x = randn(dimension,numData) ;
      t.mu = randn(dimension,numComponents) ;
      t.sigma2 = ones(dimension,numComponents) ;
      t.prior = ones(1,numComponents) ;
      t.prior = t.prior / sum(t.prior) ;
    end
  end
  
  methods (Test)
    function test_basic(t)
      phi_ = t.simple_fisher(t.x, t.mu, t.sigma2, t.prior) ;
      phi = vl_fisher(t.x, t.mu, t.sigma2, t.prior) ;
      t.verifyEqual(phi, phi_, 'absTo', 1e-10) ;
    end
    function test_norm(t)
      phi_ = t.simple_fisher(t.x, t.mu, t.sigma2, t.prior) ;
      phi_ = phi_ / norm(phi_) ;
      phi = vl_fisher(t.x, t.mu, t.sigma2, t.prior, 'normalized') ;
      t.verifyEqual(phi, phi_, 'absTo', 1e-10) ;
    end
    function test_sqrt(t)
      phi_ = t.simple_fisher(t.x, t.mu, t.sigma2, t.prior) ;
      phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
      phi = vl_fisher(t.x, t.mu, t.sigma2, t.prior, 'squareroot') ;
      t.verifyEqual(phi, phi_, 'absTo', 1e-10) ;
    end
    function test_improved(t)
      phi_ = t.simple_fisher(t.x, t.mu, t.sigma2, t.prior) ;
      phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
      phi_ = phi_ / norm(phi_) ;
      phi = vl_fisher(t.x, t.mu, t.sigma2, t.prior, 'improved') ;
      t.verifyEqual(phi, phi_, 'absTo', 1e-10) ;
    end
    function test_fast(t)
      phi_ = t.simple_fisher(t.x, t.mu, t.sigma2, t.prior, true) ;
      phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
      phi_ = phi_ / norm(phi_) ;
      phi = vl_fisher(t.x, t.mu, t.sigma2, t.prior, 'improved', 'fast') ;
      t.verifyEqual(phi, phi_, 'absTo', 1e-10) ;
    end
  end
  methods (Static)
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
    end
  end
end
