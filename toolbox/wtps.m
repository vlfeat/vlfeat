function [xp1,xp2]=wtps(phi,Yp)
% WTPS  Thin-plate spline warping
%   [XP1,XP2]=WTPS(PHI,YP) computes the thin-plate spline (TPS)
%   specified by the basis PHI and the warped control point Yp.
%
%   Yp is a 2xK matrix with one column per control point and the basis
%   PHI is calculated by means of the TPS function.
%
%   The thin-palte spline is defined on a domain X1,X2 and specified
%   by a set of points Y and their warp YP. The spline passes
%   interpolates exaclty the control points. 
%
%   The parameters X1,X2 and Y are used to compute the basis PHI. This
%   operation is fairily slow, but computing the spline for a given Yp
%   is then very quick, as the operation is just a linear combination
%   of the basis.
%
%   Examples: [xp1,xp2]=WTPS(TPS(x1,x2,Y),Yp).
%
%   See also TPS.

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

[K,M,N] = size(phi) ;

Xp=[Yp, zeros(2,3)]*reshape(phi,K,M*N) ;

xp1 = reshape(Xp(1,:),M,N) ;
xp2 = reshape(Xp(2,:),M,N) ;
