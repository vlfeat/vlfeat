function J = vl_imdown(I,method)
% VL_IMDOWN  Downsample image 
%   J = VL_IMDOWN(I,'sample') downsamples the image I by half by
%   discarding each other pixel. This is the default downsampling
%   method.
%   
%   VL_IMDOWN(I,'avg') downsamples by averaging groups of four pixels.
%   
%   See also VL_IMUP(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if nargin < 2
  method = 'sample' ;
end

switch lower(method)
  case 'sample'
    J = I(1:2:floor(end-.5),1:2:floor(end-.5),:) ;
    
  case 'avg'
    Iclass = class(I);
    I = double(I);
    J = ...
        I(1:2:end-1,1:2:end-1,:) + ...
        I(2:2:end,1:2:end-1,:) + ...
        I(1:2:end-1,2:2:end,:) + ...
        I(2:2:end,2:2:end,:) ;
    J = J / 4 ;
    eval(sprintf('J=%s(J);', Iclass));
    
  otherwise
    error(sprintf('Unknown downsampling method ''%s''.', method)) ;
end
