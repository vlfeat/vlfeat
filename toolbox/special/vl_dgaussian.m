function y = vl_dgaussian(x)
% VL_DGAUSSIAN  Derivative of Gaussian density
%   Y=VL_DGAUSSIAN(X) evaluates the derivative of the standard Gaussian
%   density.
%
%   To obtain the Gaussian density of standard deviation S, do
%
%     Y = 1/S^2 * VL_DGAUSSIAN(X/S) .
%
%   See also:: VL_GAUSSIAN(), VL_DDGAUSSIAN(), VL_HELP().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

y = -x/sqrt(2*pi) .* exp(-0.5*x.^2) ;
