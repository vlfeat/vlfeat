function J=imsc(I)
% IMSC  Scale image
%   J=IMSC(I) scales the range of the gray-scale or color image I to
%   fit in the interval [0,1].
%
%   See also:: HELP_VLFEAT()

% AUTORIGHTS

if ~isfloat(I)
  I = im2double(I) ;
end

J = I - min(I(:)) ;
J = J / max(J(:)) ;
