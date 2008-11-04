function d = vl_numder(func, x, varargin)
% VL_NUMDER  Numerical derivative
%   D = VL_NUMDER(FUNC, X) computes the numerical derivative of 
%   the function FUNC at point X.
%
%   D = VL_NUMDER(FUNC, X, ARG1, ARG2, ...) allow to pass extra
%   parameters to the function FUNC.
%
%   See also:: VL_NUMDER2(), VL_HELP().

% TODO: uniform sacaling of axis is not a good idea

dx=1e-7 ;

N=length(x) ;
fx = feval(func, x, varargin{:}) ;
d=zeros(length(fx(:)),N) ;
for n=1:N
  e=zeros(size(x)) ; e(n)=1 ;
  fxn = feval(func, x+dx*e, varargin{:}) ;
  di = (fxn-fx)/dx ;
  d(:,n) = di(:) ; 
end
