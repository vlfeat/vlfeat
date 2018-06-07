classdef vl_test_homkermap < matlab.unittest.TestCase
  
  methods (Test)
    function test_uniform_kchi2(t),     t.check('kchi2', 3,  'uniform', 15) ;end
    function test_uniform_kjs(t),       t.check('kjs',   3,  'uniform', 15) ;end
    function test_uniform_kl1(t),       t.check('kl1',  29,  'uniform', 15) ;end
    function test_rect_kchi2(t),        t.check('kchi2', 3,  'rectangular', 15) ;end
    function test_rect_kjs(t),          t.check('kjs',   3,  'rectangular', 15) ;end
    function test_rect_kl1(t),          t.check('kl1',  29,  'rectangular', 10) ;end
    function test_auto_uniform_kchi2(t),t.check('kchi2', 3,  'uniform') ;end
    function test_auto_uniform_kjs(t),  t.check('kjs',   3,  'uniform') ;end
    function test_auto_uniform_kl1(t),  t.check('kl1',  25,  'uniform') ;end
    function test_auto_rect_kchi2(t),   t.check('kchi2', 3,  'rectangular') ;end
    function test_auto_rect_kjs(t),     t.check('kjs',   3,  'rectangular') ;end
    function test_auto_rect_kl1(t),     t.check('kl1',  25,  'rectangular') ;end
    
    function test_gamma(t)
      x = linspace(0,1,20) ;
      for gamma = linspace(.2,2,10)
        k = vl_alldist(x, 'kchi2') .* (x'*x + 1e-12).^((gamma-1)/2) ;
        psix = vl_homkermap(x, 3, 'kchi2', 'gamma', gamma) ;
        t.verifyTrue(norm(k - psix'*psix) < 1e-2) ;
      end
    end
    
    function test_negative(t)
      x = linspace(-1,1,20) ;
      k = vl_alldist(abs(x), 'kchi2') .* (sign(x)'*sign(x)) ;
      psix = vl_homkermap(x, 3, 'kchi2') ;
      t.verifyTrue(norm(k - psix'*psix) < 1e-2) ;
    end
  end

  methods
    function check(t, ker, n, window, period)
      args = {n, ker, 'window', window} ;
      if nargin > 4
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
        t.verifyEqual(k, k_, 'AbsTol', cast(2e-2,'like',k)) ;
      end
    end
  end
end