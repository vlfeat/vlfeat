function [phi,S] = tps(x1,x2,Y)
% TPS  Compute the thin-plate spline basis
%   PHI=TPS(X1,X2,Y) returns the basis PHI of a thin-plate spline
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
%   [PHI,S] = TPS(X1,X2,Y) additionally returns the stiffness matrix S
%   of the TPS.
%
%   See also WTPS.

% AUTORIGHTS
% Copyright (C) 2006 Andrea Vedaldi
%       
% This file is part of VLUtil.
% 
% VLUtil is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software Foundation,
% Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

X = [x1(:)';x2(:)'] ;

K = size(Y,2) ;
Q = size(X,2) ;
U = tpsu(Y,Y) ;
L = [[ones(1,K); Y], zeros(3) ; U, ones(K,1), Y'] ;
invL = inv(L) ;

tmp = tpsu(Y,X) ;
phi = invL * [ ones(1,Q) ; X(1,:) ; X(2,:) ; tmp ] ;

[M,N] = size(x1) ;
phi = reshape(phi,K+3,M,N) ;

if nargout > 1
  % See Bookstein; note that here the terms are re-arranged a bit
  invLn = invL(1:K, end-K+1:end) ;
  S = invLn * U * invLn ;		
end