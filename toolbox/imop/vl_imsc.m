function J=vl_imsc(I)
% VL_IMSC  Scale image
%   J=VL_IMSC(I) scales the range of the gray-scale or color image I to
%   fit in the interval [0,1].
%
%   See also:: VL_HELP()

% AUTORIGHTS

if ~isfloat(I)
  I = im2double(I) ;
end

J = I - min(I(:)) ;
J = J / max(J(:)) ;
