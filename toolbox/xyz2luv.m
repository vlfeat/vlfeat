function J=xyz2luv(I,illuminant)
% XYZ2LUV  Convert XYZ to LUV
%   J = XYZ2LUV(I) converts the image I in XYZ format to the Luv
%   format (using by default the illuminant E).
%
%   J = XYZ2LUV(I,illuminant) uses the specified illuminant. The
%   following illuminant are supported: A, B, C, E, D50, D55, D65,
%   D75, D93.
%
%   See also XYZ2LAB().

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

if nargin < 2
  illuminant='E' ;
end

switch illuminant
  case 'A'
    xw = 0.4476 ;
    yw = 0.4074 ;  
  case 'B'
    xw = 0.3324 ;
    yw = 0.3474 ;
  case 'C' 
    xw = 0.3101 ;
    yw = 0.3162 ;
  case 'E'
    xw = 1/3 ;
    yw = 1/3 ;
  case 'D50'
    xw = 0.3457 ;
    yw = 0.3585 ;
  case 'D55'
    xw = 0.3324 ;
    yw = 0.3474 ;
  case 'D65'
    xw = 0.312713 ;
    yw = 0.329016 ;
  case 'D75'
    xw = 0.299 ;
    yw = 0.3149 ;
  case 'D93'
    xw = 0.2848 ;
    yw = 0.2932 ;
end

J=zeros(size(I)) ;

% Reference white
xw = 1/3 ;
yw = 1/3 ;
Yw = 1.0 ;
Xw = xw/yw ;
Zw = (1-xw-yw)/yw * Yw ;

J=zeros(size(I)) ;

X = I(:,:,1) ;
Y = I(:,:,2) ;
Z = I(:,:,3) ;

upw = 4*Xw / (Xw + 15*Yw + 3*Zw) ;
vpw = 9*Yw / (Xw + 15*Yw + 3*Zw) ;

up = 4*X ./ (X + 15*Y + 3*Z) ;
vp = 9*Y ./ (X + 15*Y + 3*Z) ;

sp = find( Y / Yw  > 0.008856) ;
sm = find( Y / Yw <= 0.008856) ;

L = zeros(size(Y)) ;
L(sp) =   116*( Y(sp) / Yw ).^(1/3) - 16 ;
L(sm) = 903.3*( Y(sm) / Yw ) ;

u = 13 * L .* (up - upw) ;
v = 13 * L .* (vp - vpw) ;

J = cat(3,L,u,v) ;
