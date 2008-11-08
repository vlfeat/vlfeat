function y = vl_dsigmoid(x)
% VL_DSIGMOID Derivative of vl_sigmoid function
%   Y = VL_DSIGMOID(X) returns the derivative of VL_SIGMOID(X). This is
%   calculated as - VL_SIGMOID(X) * (1 - VL_SIGMOID(X)).
%
%   See also:: VL_SIGMOID(X), VL_HELP().

t = vl_sigmoid(x) ;
y = t .* (1 - t) ;
