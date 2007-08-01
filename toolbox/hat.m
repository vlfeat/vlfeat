function H = hat(om)
% HAT  Hat operator
%   H = HAT(OM) returns the skew symmetric matrix by taking the "hat"
%   of the 3D vector OM.
%  
%   See also IHAT().

H = [
	0      -om(3)  om(2) ;
	om(3)  0      -om(1) ;
	-om(2) om(1)   0     ] ;
