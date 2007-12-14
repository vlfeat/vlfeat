function J=imsc(I)
% IMSC  Scale image
%   J=IMSC(I) scales the ranges of the gray-scale or color image I
%   to fit in the interval [0,1].
%
%   IMSC works only on floating point images.

if(~isfloat(I))
  error('I must be float') ;
end

J = I - min(I(:)) ;
J = J / max(J(:)) ;
