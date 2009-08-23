function d = vl_numder(func, x, varargin)
% VL_NUMDER  Numerical derivative
%   D = VL_NUMDER(FUNC, X) computes the numerical derivative of the
%   function FUNC at point X. X is a real array and is passed as first
%   argument of FUNC.
%
%   D = VL_NUMDER(FUNC, X, ARG1, ARG2, ...) passes ARG1, ARG2, ...  as
%   additional arguments to the function FUNC.
%
%   See also:: VL_NUMDER2(), VL_HELP().

% TODO: uniform sacaling of axis is not a good idea

dx = 1e-7 ;
N  = numel(x) ;
fx = feval(func, x, varargin{:}) ;
d  = zeros(length(fx(:)),N) ;

for n=1:N
  e   = zeros(size(x)) ; e(n) = 1 ;
  fxn = feval(func, x+dx*e, varargin{:}) ;
  di  = (fxn - fx) / dx ;
  d(:,n) = di(:) ;
end
