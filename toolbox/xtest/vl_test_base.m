classdef vl_test_base < matlab.unittest.TestCase
  methods
    function der(test, g, x, dzdy, dzdx, delta, tau)
      if nargin < 7
        tau = [] ;
      end
      dzdx_ = test.toDataType(test.numder(g, x, dzdy, delta)) ;
      test.eq(gather(dzdx_), gather(dzdx), tau) ;
    end
    
    function eq(test,a,b,tau)
      a = gather(a) ;
      b = gather(b) ;
      if nargin > 3 && ~isempty(tau) && tau < 0
        tau_min = -tau ;
        tau = [] ;
      else
        tau_min = 0 ;
      end
      if nargin < 4 || isempty(tau)
        maxv = max([max(a(:)), max(b(:))]) ;
        minv = min([min(a(:)), min(b(:))]) ;
        tau = max(1e-2 * (maxv - minv), 1e-3 * max(maxv, -minv)) ;
      end
      tau = max(tau, tau_min) ;
      if isempty(tau) % can happen if a and b are empty
        tau = 0 ;
      end
      test.verifyThat(b, matlab.unittest.constraints.IsOfClass(class(a))) ;
      tau = feval(class(a), tau) ; % convert to same type as a
      tol = matlab.unittest.constraints.AbsoluteTolerance(tau) ;
      test.verifyThat(a, matlab.unittest.constraints.IsEqualTo(b, 'Within', tol)) ;
    end
  end
end