function y = sigmoid(x)
% SIGMOID Sigmoid function
%   Y = SIGMOID(X) returns
%
%    Y = 1 ./ (1 + EXP(X)) ;
%
%   REMARKS. Useful properties of the sigmoid are:
%
%   -  1 - SIGMOID(X) = SIGMOID(-X)
%   -  Centered sigmoid: 2 * SIGMOID(X) - 1 ;
%   -  SIGMOID(X) = (EXP(X/2) - EXP(X/2)) / (EXP(X/2) + EXP(X/2))
% 
%  See also DSIGMOID(X).

% AUTORIGHTS

y = 1 ./ (1 + exp(-x)) ;

