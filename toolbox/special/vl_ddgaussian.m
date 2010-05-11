function y = vl_ddgaussian(x)
% VL_DDGAUSSIAN Second derivative of Gaussian density
%   Y=VL_DDGAUSSIAN(X) computes the second derivative of the standard
%   Gaussian density.
%
%   To obtain the second derivative of the Gaussian density of
%   standard deviation S, do
%
%     Y = 1/S^3 * VL_DDGAUSSIAN(X/S) .
%
%   See also:: VL_GAUSSIAN(), VL_DGAUSSIAN(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

y = (x.^2 - 1)/sqrt(2*pi) .* exp(-0.5*x.^2) ;
