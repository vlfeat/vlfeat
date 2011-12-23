function y = vl_rcos(r,x)
% VL_RCOS RCOS function
%   Y = VL_RCOS(R,X) computes the RCOS function with roll-off R.
%
%   See also: VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

x = abs(x) ;
if(r > 0)
  y = (x < (1 - r)) + ...
      0.5 * (1 + cos((1 - r - x)/r*pi)) .* ...
      (x <= 1) .* (x >= 1 - r) ;
else
  y = (abs(x) <= 1) ;
end
