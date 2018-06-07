classdef vl_test_alldist2 < matlab.unittest.TestCase
  properties
    s = [] ;
  end
  
  methods (TestClassSetup)
    function setup(t)
      vl_twister('state', 0) ;
      t.s.X = 3.1 * vl_twister(10,10) ;
      t.s.Y = 4.7 * vl_twister(10,7) ;
    end
  end
  
  methods (Test)
    function test_null_args(t)
      t.verifyEqual(...
        vl_alldist2(zeros(15,12), zeros(15,0), 'kl2'), ...
        zeros(12,0)) ;
      
      t.verifyEqual(...
        vl_alldist2(zeros(15,0), zeros(15,0), 'kl2'), ...
        zeros(0,0)) ;
      
      t.verifyEqual(...
        vl_alldist2(zeros(15,0), zeros(15,12), 'kl2'), ...
        zeros(0,12)) ;
      
      t.verifyEqual(...
        vl_alldist2(zeros(0,15), zeros(0,12), 'kl2'), ...
        zeros(15,12)) ;
    end
    
    function test_self(t)
      t.verifyEqual(...
        vl_alldist2(t.s.X, 'kl2'), ...
        t.makedist(@(x,y) x*y, t.s.X, t.s.X), ...
        'absTol',1e-6) ;
    end
    
    function test_distances(t)
      dists = {'chi2', 'l2', 'l1', 'hell', ...
        'kchi2', 'kl2', 'kl1', 'khell'} ;
      distsEquiv = { ...
        @(x,y) (x-y)^2 / (x + y), ...
        @(x,y) (x-y)^2, ...
        @(x,y) abs(x-y), ...
        @(x,y) (sqrt(x) - sqrt(y))^2, ...
        @(x,y) 2 * (x*y) / (x + y), ...
        @(x,y) x*y, ...
        @(x,y) min(x,y), ...
        @(x,y) sqrt(x.*y)};
      types = {'single', 'double', 'sparse'} ;
      
      for simd = [0 1]
        for d = 1:length(dists)
          for q = 1:length(types)
            vl_simdctrl(simd) ;
            X = feval(str2func(types{q}), t.s.X) ;
            Y = feval(str2func(types{q}), t.s.Y) ;
            a = vl_alldist2(X,Y,dists{d}) ;
            b = t.makedist(distsEquiv{d},X,Y) ;
            t.verifyEqual(a,b, ...
              'absTol', cast(1e-4,'like',a), ...
              sprintf('alldist failied for dist=%s type=%s simd=%d', ...
              dists{d}, ...
              types{q}, ...
              simd)) ;
          end
        end
      end
    end
    
    function test_distance_kernel_pairs(t)
      dists = {'chi2', 'l2', 'l1', 'hell'} ;
      for d = 1:length(dists)
        dist = char(dists{d}) ;
        X = t.s.X ;
        Y = t.s.Y ;
        ker = ['k' dist] ;
        kxx = vl_alldist2(X,X,ker) ;
        kyy = vl_alldist2(Y,Y,ker) ;
        kxy = vl_alldist2(X,Y,ker) ;
        kxx = repmat(diag(kxx), 1, size(t.s.Y,2)) ;
        kyy = repmat(diag(kyy), 1, size(t.s.X,1))' ;
        d2  = vl_alldist2(X,Y,dist) ;
        t.verifyEqual(d2, kxx + kyy - 2 * kxy, 'AbsTol', 1e-6) ;
      end
    end
  end
  
  methods (Static)
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
    end
  end
end

