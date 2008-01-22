function J = imdown(I,method)
% IMDOWN  Downsample image 
%   J = IMDOWN(I,'sample') downsamples the image I by half by
%   discarding each other pixel.
%   
%   IMDOWN(I,'avg') downsmples by averaging groups of four pixels.
%
%   The image is always converted to double format.
%   
%   See also IMUP().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

if nargin < 2
  method = 'sample' ;
end

switch method
  case 'sample'
    J = I(1:2:end,1:2:end,:) ;
    
  case 'avg'
    J = ...
        I(1:2:end-1,1:2:end-1,:) + ...
        I(2:2:end,1:2:end-1,:) + ...
        I(1:2:end-1,2:2:end,:) + ...
        I(2:2:end,2:2:end,:) ;
    J = J / 4 ;
end
