function J = vl_imup(I)
% VL_IMUP Upsample an image by two
%   J=VL_IMUP(I) doubles the resolution of the image I by using
%   bilinear interpolation.
%
%   See also: VL_IMDOWN(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

[M,N,K] = size(I) ;

J = zeros(2*M,2*N,K) ;

J(1:2:end,1:2:end,:) = I ;

J(2:2:end,1:2:end,:) = 0.5*(I+[I(2:end,:,:);I(end,:,:)]) ;
J(1:2:end,2:2:end,:) = 0.5*(I+[I(:,2:end,:),I(:,end,:)]) ;
J(2:2:end,2:2:end,:) = ...
  0.25*(...
  J(2:2:end,1:2:end-1,:)+...
  J(1:2:end-1,2:2:end,:)+...
  [J(2:2:end,3:2:end,:),J(2:2:end,end-1,:)]+...
  [J(3:2:end,2:2:end,:);J(end-1,2:2:end,:)]) ;
