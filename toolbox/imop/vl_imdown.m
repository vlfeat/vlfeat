function J = vl_imdown(I, varargin)
% VL_IMDOWN  Downsample an image by two
%   J = VL_IMDOWN(I) downsamples the image I by half by discarding
%   each other pixel.
%
%   VL_IMDOWN() accepts the following options:
%
%   Method:: Sample
%     'Sample' downsamples the image by discarding
%     pixels. 'Average' instead averages groups of 2x2 pixels.
%
%   See also: VL_IMUP(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

opts.method = 'sample' ;
opts = vl_argparse(opts, varargin) ;

switch lower(opts.method)
  case 'sample'
    J = I(1:2:floor(end-.5),1:2:floor(end-.5),:) ;

  case 'average'
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
    error('Unknown downsampling method ''%s''.', method) ;
end
