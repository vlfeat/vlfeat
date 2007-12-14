function [dWx,dWy] = dwaffine(x,y)
% DWAFFINE  Derivative of affine warp
%   [DWX,DWY]=DWAFFINE(X,Y) returns the derivative of the 2-D affine
%   warp [WX; WY] = [A T] [X; Y] with respect to the parameters A,T
%   computed at points X,Y.
%
%   See also WAFFINE().

% dW = [ kron(x',I) I ]
%    | 
%    = [ x1  0  x2  0 1 0 ] 
%      [  0 x1   0 x2 0 1 ]

z = zeros(length(x(:)),1) ;
o =  ones(length(x(:)),1) ;

dWx = [ x(:) z      y(:) z      o z ] ;
dWy = [ z    x(:)   z    y(:)   z o ] ;
