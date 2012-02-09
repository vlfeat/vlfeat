function y = vl_dsigmoid(x)
% VL_DSIGMOID  Derivative of the sigmoid function
%   Y = VL_DSIGMOID(X) returns the derivative of VL_SIGMOID(X). This is
%   calculated as - VL_SIGMOID(X) * (1 - VL_SIGMOID(X)).
%
%   See also: VL_SIGMOID(X), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

t = vl_sigmoid(x) ;
y = t .* (1 - t) ;
