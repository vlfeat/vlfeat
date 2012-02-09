function J=vl_rgb2xyz(I,ws)
% VL_RGB2XYZ  Convert RGB color space to XYZ
%   J=VL_RGB2XYZ(I) converts the CIE RGB image I to the image J in
%   CIE XYZ format. CIE RGB has a white point of R=G=B=1.0
%
%   VL_RGB2XYZ(I,WS) uses the specified RGB working space WS. The
%   function supports the following RGB working spaces:
%
%   * `CIE'    E illuminant, gamma=2.2
%   * `Adobe'  D65 illuminant, gamma=2.2
%
%   The default workspace is CIE.
%
%   See also: VL_XYZ2RGB(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

[M,N,K] = size(I) ;

if K~=3
	error('I must be a MxNx3 array.') ;
end

I=im2double(I) ;

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

I = reshape(I.^gamma, M*N, K) ;
J = A*I' ;
J = reshape(J', M, N, K) ;
