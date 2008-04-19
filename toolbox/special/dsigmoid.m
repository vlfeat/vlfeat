function y = dsigmoid(x)
% DSIGMOID Derivative of sigmoid function
%   Y = DSIGMOID(X) returns the derivative of SIGMOID(X). This is
%   calculated as - SIGMOID(X) * (1 - SIGMOID(X)).
%
%   See also:: SIGMOID(X), HELP_VLFEAT().

t = sigmoid(x) ;
y = t .* (1 - t) ;
