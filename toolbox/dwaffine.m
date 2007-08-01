function [dWx,dWy] = dwaffine(x,y,A,T)
% DWAFFINE  Derivative of affine warp
%   dW=DWAFFINE(X,Y,A,T) returns the derivative of the affine
%   warp with respect to the parameters A,T.
%
%   See also XXX.

% dW = [ kron(x',I) I ]
%    | 
%    = [ x1  0  x2  0 1 0 ] 
%      [  0 x1   0 x2 0 1 ]

z = zeros(length(x(:)),1) ;
o =  ones(length(x(:)),1) ;

dWx = [ x(:) z      y(:) z      o z ] ;
dWy = [ z    x(:)   z    y(:)   z o ] ;
