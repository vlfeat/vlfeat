% RODR  Rodrigues' formula
%   R = RODR(OM) where OM a 3-dimensional column vector computes the
%   Rodrigues' formula of OM, returning the rotation matrix R =
%   expm(hat(OM)).
%
%   [R,DR] = RODR(OM) computes also the derivative of the Rodrigues
%   formula. In matrix notation this is the expression
%
%           d(vec expm(hat(OM)) )
%     dR = ----------------------.
%                  d om^T
%
%   [R,DR]=RODR(OM) when OM is a 3xK matrix repeats the operation for
%   each column (or equivalently matrix with 3*K elements). In this
%   case R and DR are arrays with K slices, one per rotation.
%
%   COMPATIBILITY. Some code uses the RODRIGUES() function. This
%   function is very similar, except for the format of the derivative,
%   which differs by a permutation of the elements.
%
%   See also IRODR(), PDF:RODRIGUES.
