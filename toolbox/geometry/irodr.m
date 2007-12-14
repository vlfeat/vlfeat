% IRODR  Inverse Rodrigues' formula
%   OM = IRODR(R) where R is a rotation matrix computes the the
%   inverse Rodrigues' formula of om, returning the rotation matrix R
%   = dehat(Logm(OM)).
%
%   [OM,DOM] = IRODR(R) computes also the derivative of the Rodrigues'
%   formula. In matrix notation this is the expression
%
%          d( dehat logm(hat(R)) )
%     dom = ----------------------.
%                  d(vec R)^T
%
%   [OM,DOM] = IRODR(R) when R is a 9xK matrix repeats the operation
%   for each column (or equivalently matrix with 9*K elements). In
%   this case OM and DOM are arrays with K slices, one per rotation.
%
%   COMPATIBILITY. Some code uses the RODRIGUES() function. This
%   function is very similar, except for the format of the derivative,
%   which differs by a permutation of the elements.
%
%   See also RODR().
