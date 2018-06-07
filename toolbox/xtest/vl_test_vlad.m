classdef vl_test_vlad < matlab.unittest.TestCase
  properties
    x
    mu
    assignments
  end

  methods (TestClassSetup)
    function setup(t)
      randn('state',0) ;
      t.x = randn(128,256) ;
      t.mu = randn(128,16) ;
      assignments = rand(16, 256) ;
      t.assignments = bsxfun(@times, assignments, 1 ./ sum(assignments,1)) ;
    end
  end

  methods (Test)
    function test_basic(t)
      x = [1, 2, 3] ;
      mu = [0, 0, 0] ;
      assignments = eye(3) ;
      phi = vl_vlad(x, mu, assignments, 'unnormalized') ;
      t.verifyEqual(phi, [1 2 3]') ;
      mu = [0, 1, 2] ;
      phi = vl_vlad(x, mu, assignments, 'unnormalized') ;
      t.verifyEqual(phi, [1 1 1]') ;
      phi = vl_vlad([x x], mu, [assignments assignments], 'unnormalized') ;
      t.verifyEqual(phi, [2 2 2]') ;
    end
    function test_rand(t)
      phi_ = t.simple_vlad(t.x, t.mu, t.assignments) ;
      phi = vl_vlad(t.x, t.mu, t.assignments, 'unnormalized') ;
      t.verifyEqual(phi, phi_, 'AbsTol', 1e-4) ;
    end
    function test_norm(t)
      phi_ = t.simple_vlad(t.x, t.mu, t.assignments) ;
      phi_ = phi_ / norm(phi_) ;
      phi = vl_vlad(t.x, t.mu, t.assignments) ;
      t.verifyEqual(phi, phi_, 'AbsTo', 1e-4) ;
    end
    function test_sqrt(t)
      phi_ = t.simple_vlad(t.x, t.mu, t.assignments) ;
      phi_ = sign(phi_) .* sqrt(abs(phi_)) ;
      phi_ = phi_ / norm(phi_) ;
      phi = vl_vlad(t.x, t.mu, t.assignments, 'squareroot') ;
      t.verifyEqual(phi, phi_, 'AbsTo', 1e-4) ;
    end
    function test_individual(t)
      phi_ = t.simple_vlad(t.x, t.mu, t.assignments) ;
      phi_ = reshape(phi_, size(t.x,1), []) ;
      phi_ = bsxfun(@times, phi_, 1 ./ sqrt(sum(phi_.^2))) ;
      phi_ = phi_(:) ;
      phi = vl_vlad(t.x, t.mu, t.assignments, 'unnormalized', 'normalizecomponents') ;
      t.verifyEqual(phi, phi_, 'AbsTo', 1e-4) ;
    end
    function test_mass(t)
      phi_ = t.simple_vlad(t.x, t.mu, t.assignments) ;
      phi_ = reshape(phi_, size(t.x,1), []) ;
      phi_ = bsxfun(@times, phi_, 1 ./ sum(t.assignments,2)') ;
      phi_ = phi_(:) ;
      phi = vl_vlad(t.x, t.mu, t.assignments, 'unnormalized', 'normalizemass') ;
      t.verifyEqual(phi, phi_, 'AbsTo', 1e-4) ;
    end
  end

  methods (Static)
    function enc = simple_vlad(x, mu, assign)
      for i = 1:size(assign,1)
        enc{i} = x * assign(i,:)' - sum(assign(i,:)) * mu(:,i) ;
      end
      enc = cat(1, enc{:}) ;
    end
  end
end
