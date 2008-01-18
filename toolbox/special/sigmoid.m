function y = sigmoid(x)
% SIGMOID Sigmoind function
%  Y = SIGMOID(X) returns
%
%   Y = 1 ./ (1 + EXP(X)) ;
%
%  REMARKS. It is useful to recall that
%
%  *   1 - SIGMOID(X) = SIGMOID(-X)
%  *   Centered sigmoid: 2 * SIGMOID(X) - 1 ;
%  *   SIGMOID(X) = (EXP(X/2) - EXP(X/2)) / (EXP(X/2) + EXP(X/2))
% 
%  See also DSIGMOID(X)  

y = 1 ./ (1 + exp(-x)) ;

