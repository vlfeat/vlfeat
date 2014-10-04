function J=vl_imsc(I)
% VL_IMSC  Scale image
%   J=VL_IMSC(I) scales the range of the gray-scale or color image I to
%   fit in the interval [0,1].
%
%   See also: VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if ~isfloat(I)
  I = im2double(I) ;
end

J = I - min(I(:)) ;
J = J / max(J(:)) ;
J = max(min(J, 1), 0) ;
