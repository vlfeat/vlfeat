function J=vl_xyz2luv(I,il)
% VL_XYZ2LUV  Convert XYZ color space to LUV
%   J = VL_XYZ2LUV(I) converts the image I in XYZ format to the image J
%   in Luv format.
%
%   J = VL_XYZ2LUV(I,IL) uses the specified illuminant. The following
%   illuminant are supported: A, B, C, E, D50, D55, D65, D75, D93.  IL
%   is the name of the illuminant.
%
%   See also:: VL_XYZ2LAB(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if nargin < 2
  il='E' ;
end

switch lower(il)
  case 'a'
    xw = 0.4476 ;
    yw = 0.4074 ;  
  case 'b'
    xw = 0.3324 ;
    yw = 0.3474 ;
  case 'c' 
    xw = 0.3101 ;
    yw = 0.3162 ;
  case 'e'
    xw = 1/3 ;
    yw = 1/3 ;
  case 'd50'
    xw = 0.3457 ;
    yw = 0.3585 ;
  case 'd55'
    xw = 0.3324 ;
    yw = 0.3474 ;
  case 'd65'
    xw = 0.312713 ;
    yw = 0.329016 ;
  case 'd75'
    xw = 0.299 ;
    yw = 0.3149 ;
  case 'd93'
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
