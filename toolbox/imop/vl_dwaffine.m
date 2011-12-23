function [dWx,dWy] = vl_dwaffine(x,y)
% VL_DWAFFINE  Derivative of an affine warp
%   [DWX,DWY]=VL_DWAFFINE(X,Y) returns the derivative of the 2-D affine
%   warp [WX; WY] = [A T] [X; Y] with respect to the parameters A,T
%   computed at points X,Y.
%
%   See also: VL_WAFFINE(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% dW = [ kron(x',I) I ]
%    |
%    = [ x1  0  x2  0 1 0 ]
%      [  0 x1   0 x2 0 1 ]

z = zeros(length(x(:)),1) ;
o =  ones(length(x(:)),1) ;

dWx = [ x(:) z      y(:) z      o z ] ;
dWy = [ z    x(:)   z    y(:)   z o ] ;
