function y = vl_dgaussian(x)
% VL_DGAUSSIAN  Derivative of the Gaussian density function
%   Y=VL_DGAUSSIAN(X) evaluates the derivative of the standard Gaussian
%   density.
%
%   To obtain the Gaussian density of standard deviation S, do
%
%     Y = 1/S^2 * VL_DGAUSSIAN(X/S) .
%
%   See also: VL_GAUSSIAN(), VL_DDGAUSSIAN(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

y = -x/sqrt(2*pi) .* exp(-0.5*x.^2) ;
