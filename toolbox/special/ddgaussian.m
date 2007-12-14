function y = ddgaussian(x)
% DDGAUSSIAN Second derivative of Gaussian density
%   Y=DDGAUSSIAN(X) computes the second derivative of the standard
%   Gaussian density.
%
%   To obtain the second derivative of the Gaussian density of
%   standard deviation S, do
%
%     Y = 1/S^3 * DDGAUSSIAN(X/S) .
%
%   See also GAUSSIAN(), DGAUSSIAN().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

y = (x.^2 - 1)/sqrt(2*pi) .* exp(-0.5*x.^2) ;
