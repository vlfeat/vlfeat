function H = vl_hat(om)
% VL_HAT  Hat operator
%   H = VL_HAT(OM) returns the skew symmetric matrix by taking the "hat"
%   of the 3D vector OM.
%
%   See also: VL_IHAT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

H = [0      -om(3)  om(2) ;
     om(3)  0      -om(1) ;
     -om(2) om(1)   0     ] ;
