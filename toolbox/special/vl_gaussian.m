function y = vl_gaussian(x)
% VL_GAUSSIAN  Standard Gaussian density
%   Y=VL_GAUSSIAN(X) computes the standard (zero mean, unit variance)
%   Gaussian density.  
%
%   To obtain the Gaussian density of standard deviation S do
%
%     Y = 1/S * VL_GAUSSIAN(X/S).
%
%   See also:: VL_DGAUSSIAN(), VL_DDGAUSSIAN(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

y = 1/sqrt(2*pi)*exp(-0.5*x.^2) ;

