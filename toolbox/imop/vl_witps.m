function [x1,x2]=vl_witps(xp1,xp2,Y,Yp)
% VL_WITPS  Inverse thin-plate spline warping
%   [X1,X2]=VL_WITPS(XP1,XP2,Y,Yp) computes the inverse thin-plate spline
%   (TPS) warp of the points XP1,XP2.
%
%   Remark::
%     The inverse of a thin-plate spline in general is NOT a
%     thin-plate spline and some splines do not have an inverse.  This
%     function uses Gauss-Newton to compute a set of points (X1,X2)
%     such that [XP1,XP2]=VL_WTPS(X1,X2,Y,Yp).
%
%   See also: VL_WTPS(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% Initial guess by inverting the control points
[x1,x2] = vl_wtps(vl_tps(xp1,xp2,Yp),Y) ;

X  = [x1(:)';x2(:)'] ;
Xp = [xp1(:)',;xp2(:)'] ;

% Gauss-Newton
K = size(Y,2) ;
N = size(X,2) ;
U = vl_tpsu(Y,Y) ;
L = [[ones(1,K); Y], zeros(3) ; U, ones(K,1), Y'] ;
invL = inv(L) ;
A = [Yp, zeros(2,3)] * invL ;

for t=1:5
  [U,dU]  = vl_tpsu(Y,X);
  W = A * [repmat([0 0;1 0;0 1],1,N); reshape(dU, K, 2*N)] ;
  err = Xp - A * [ ones(1,N) ; X(1,:) ; X(2,:) ; U ] ;

  W = reshape(W,4,N) ;
  dets = W(1,:).*W(4,:) - W(3,:).*W(2,:) ;
  dX = [ (  W(4,:).*err(1,:) - W(3,:).*err(2,:) ) ./ dets ; ...
         (- W(2,:).*err(1,:) + W(1,:).*err(2,:) ) ./ dets ] ;
  X = X + dX ;
end

[M,N] = size(xp1) ;
x1 = reshape(X(1,:),M,N) ;
x2 = reshape(X(2,:),M,N) ;
