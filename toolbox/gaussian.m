function y = gaussian(x)
% GAUSSIAN  Standard Gaussian density
%   Y=GAUSSIAN(X) computes the standard (zero mean, unit variance)
%   Gaussian density.  
%
%   To obtain the Gaussian density of standard deviation S do
%
%     Y = 1/S * GAUSSIAN(X/S).
%
%   See also DGAUSSIAN(), DDGAUSSIAN().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

y = 1/sqrt(2*pi)*exp(-0.5*x.^2) ;

