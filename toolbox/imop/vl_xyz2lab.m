function J=vl_xyz2lab(I,il)
% VL_XYZ2LAB  Convert XYZ color space to LAB
%   J = VL_XYZ2LAB(I) converts the image from XYZ format to LAB format.
%
%   VL_XYZ2LAB(I,IL) uses one of the illuminants A, B, C, E, D50, D55,
%   D65, D75, D93. The default illuminatn is E.
%
%   See also: VL_XYZ2LUV(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

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

% --------------------------------------------------------------------
function b=f(a)
% --------------------------------------------------------------------
sp = find(a  > 0.00856) ;
sm = find(a <= 0.00856) ;
k = 903.3 ;
b=zeros(size(a)) ;
b(sp) = a(sp).^(1/3) ;
b(sm) = (k*a(sm) + 16)/116 ;
