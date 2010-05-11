function J=vl_imsc(I)
% VL_IMSC  Scale image
%   J=VL_IMSC(I) scales the range of the gray-scale or color image I to
%   fit in the interval [0,1].
%
%   See also:: VL_HELP()

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if ~isfloat(I)
  I = im2double(I) ;
end

J = I - min(I(:)) ;
J = J / max(J(:)) ;
