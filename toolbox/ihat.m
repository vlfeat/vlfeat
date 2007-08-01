function om = ihat( H )
% IHAT Inverse hat operator
%   OM = DEHAT(H) returns a vector OM such that HAT(OM) = H.
%   H hast to be 3x3 skew-symmetric.
%
%   See also HAT().

om = [H(3,2);H(1,3);H(2,1)] ;
