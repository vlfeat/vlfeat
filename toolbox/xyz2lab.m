function J=xyz2lab(I)
% XYZ2LAB  Convert XYZ to LAB
%   J = XYZ2LAB(I) converts the image from XYZ format to LAB format.
%
%   XYZ2LAB(I,'illuminant') uses the specified illuminant. The
%   following illuminant are supported: 'A', 'B', 'C', 'E', 'D50',
%   'D55', 'D65', 'D75', 'D93'. By default, 'E' is usedd.
%
%   See also XYZ2LUV().

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

% XYZ components
X = I(:,:,1) ;
Y = I(:,:,2) ;
Z = I(:,:,3) ;

x = X/Xw ;
y = Y/Yw ;
z = Z/Zw ;

L = 116 * f(y) - 16 ;
a = 500*(f(x) - f(y)) ; 
b = 200*(f(y) - f(z)) ;

J = cat(3,L,a,b) ;

function b=f(a)
sp = find(a  > 0.00856) ;
sm = find(a <= 0.00856) ;
k = 903.3 ; 
b=zeros(size(a)) ;
b(sp) = a(sp).^(1/3) ;
b(sm) = (k*a(sm) + 16)/116 ;
