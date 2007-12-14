function J = imup(I)
% IMUP  Downsample an image 
%   J=IMUP(I) doubles the resolution of the image I by bilinear
%   interpolation.
%
%   See also IMDOWN().

% AUTORIGHTS
% Copyright (C) 2006 Andrea Vedaldi
%       
% This file is part of VLUtil.
% 
% VLUtil is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software Foundation,
% Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

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
