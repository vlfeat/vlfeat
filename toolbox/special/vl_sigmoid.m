function y = vl_sigmoid(x)
% VL_SIGMOID Sigmoid function
%   Y = VL_SIGMOID(X) returns
%
%    Y = 1 ./ (1 + EXP(X)) ;
%
%   Remark::
%     Useful properties of the sigmoid function are:
%
%     -  1 - VL_SIGMOID(X) = VL_SIGMOID(-X)
%     -  Centered sigmoid: 2 * VL_SIGMOID(X) - 1 ;
%     -  VL_SIGMOID(X) = (EXP(X/2) - EXP(X/2)) / (EXP(X/2) + EXP(X/2))
%
%   See also: VL_DSIGMOID(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

y = 1 ./ (1 + exp(-x)) ;
