function [phi,S] = vl_tps(x1,x2,Y)
% VL_TPS  Compute the thin-plate spline basis
%   PHI=VL_TPS(X1,X2,Y) returns the basis PHI of a thin-plate spline
%   (TPS) defined on the domain X1,X2 with control points Y.
%
%   X1 and X2 are MxN matrices specifying the grid vertices.  When
%   warping images, these usually correspond to image pixels.
%
%   Y is a 2xK matrix specifying the control points, one per
%   column. Ofthen Y is a subset of the domain X1,X2, but this is not
%   required.
%
%   PHI is a (K+3)xNxM matrix, with one layer per basis element. Each
%   basis element is a function of the domain X1,X2.
%
%   [PHI,S] = VL_TPS(X1,X2,Y) additionally returns the stiffness matrix S
%   of the TPS.
%
%   See also: VL_WTPS(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

X = [x1(:)';x2(:)'] ;

K = size(Y,2) ;
Q = size(X,2) ;
U = vl_tpsu(Y,Y) ;
L = [[ones(1,K); Y], zeros(3) ; U, ones(K,1), Y'] ;
invL = inv(L) ;

tmp = vl_tpsu(Y,X) ;
phi = invL * [ ones(1,Q) ; X(1,:) ; X(2,:) ; tmp ] ;

[M,N] = size(x1) ;
phi = reshape(phi,K+3,M,N) ;

if nargout > 1
  % See Bookstein; note that here the terms are re-arranged a bit
  invLn = invL(1:K, end-K+1:end) ;
  S = invLn * U * invLn ;
end
