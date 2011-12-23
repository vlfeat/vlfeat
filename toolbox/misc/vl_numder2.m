function d = vl_numder2(func, x, varargin)
% VL_NUMDER2  Numerical second derivative
%   D = VL_NUMDER2(FUNC, X) computes the numerical second derivative of
%   the function FUNC at point X.
%
%   D = VL_NUMDER2(FUNC, X, ARG1, ARG2, ...) allow to pass extra
%   parameters to the function FUNC.
%
%   See also: VL_NUMDER(), VL_HELP().

N=length(x(:)) ;
f00 = feval(func, x, varargin{:}) ;
d=zeros(length(f00),N,N) ;
D=1e-5 ;
D2=D*D ;

for n=1:N
	en=zeros(size(x)) ; en(n)=1 ;
	for m=1:N
		em=zeros(size(x)) ; em(m)=1 ;

		if( n ~= m )
			f10 = feval(func, x+en*D, varargin{:}) ;
			f01 = feval(func, x+em*D, varargin{:}) ;
			f11 = feval(func, x+en*D+em*D, varargin{:}) ;
			d(:,n,m) = (f11-f01+f00-f10)/D2 ;
		else
			fp = feval(func, x+en*D, varargin{:}) ;
			fm = feval(func, x-en*D, varargin{:}) ;
			d(:,n,m) = (fm+fp-2*f00)/D2 ;
		end
	end
end
