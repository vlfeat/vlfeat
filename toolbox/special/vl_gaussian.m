function y = vl_gaussian(x)
% VL_GAUSSIAN  Standard Gaussian density function
%   Y=VL_GAUSSIAN(X) computes the standard (zero mean, unit variance)
%   Gaussian density.
%
%   To obtain the Gaussian density of standard deviation S do
%
%     Y = 1/S * VL_GAUSSIAN(X/S).
%
%   See also: VL_DGAUSSIAN(), VL_DDGAUSSIAN(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

y = 1/sqrt(2*pi)*exp(-0.5*x.^2) ;
