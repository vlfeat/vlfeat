function y = vl_sigmoid(x)
% VL_SIGMOID Sigmoid function
%   Y = VL_SIGMOID(X) returns
%
%    Y = 1 ./ (1 + EXP(X)) ;
%
%   Remark:: 
%     Useful properties of the vl_sigmoid are:
%
%     -  1 - VL_SIGMOID(X) = VL_SIGMOID(-X)
%     -  Centered sigmoid: 2 * VL_SIGMOID(X) - 1 ;
%     -  VL_SIGMOID(X) = (EXP(X/2) - EXP(X/2)) / (EXP(X/2) + EXP(X/2))
% 
%   See also:: VL_DSIGMOID(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

y = 1 ./ (1 + exp(-x)) ;

