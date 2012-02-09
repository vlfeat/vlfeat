function J=vl_xyz2rgb(I,ws)
% VL_XYZ2RGB  Convert XYZ to RGB
%   J = VL_XYZ2RGB(I) the XYZ image I in RGB format.
%
%   VL_XYZ2RGB(I,WS) uses the RGB workspace WS. WS is a string in
%
%   - CIE:    E illuminant and 2.2 gamma
%   - Adobe:  D65 illuminant and 2.2 gamma
%
%   The default workspace is CIE.
%
%   See also: VL_RGB2XYZ(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if(nargin < 2)
  workspace = 'CIE' ;
else
  workspace = ws ;
end

switch workspace
  case 'CIE'
    % CIE: E illuminant and 2.2 gamma
    A = [
      0.488718    0.176204    0.000000
      0.310680    0.812985    0.0102048
      0.200602     0.0108109  0.989795 ]' ;
    gamma = 2.2 ;

  case 'Adobe'
    % Adobe 1998: D65 illuminant and 2.2 gamma
    A = [
      0.576700    0.297361    0.0270328
      0.185556    0.627355    0.0706879
      0.188212    0.0752847   0.99124 ]' ;
    gamma = 2.2 ;
end

[M,N,K] = size(I) ;

I = reshape(I, M*N, K) ;
J = inv(A)*I' ;
J = reshape(J'.^(1/gamma), M, N, K) ;
