function om = vl_ihat( H )
% VL_IHAT Inverse vl_hat operator
%   OM = VL_IHAT(H) returns a vector OM such that VL_HAT(OM) = H.
%   H hast to be 3x3 skew-symmetric.
%
%   See also: VL_HAT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

om = [H(3,2);H(1,3);H(2,1)] ;
