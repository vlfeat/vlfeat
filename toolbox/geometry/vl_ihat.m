function om = vl_ihat( H )
% VL_IHAT Inverse vl_hat operator
%   OM = VL_IHAT(H) returns a vector OM such that VL_HAT(OM) = H.
%   H hast to be 3x3 skew-symmetric.
%
%   See also:: VL_HAT(), VL_HELP().

om = [H(3,2);H(1,3);H(2,1)] ;
