function J=rgb2xyz(I,ws)
% RGB2XYZ  Color space conversion
%   J=RGB2XYZ(I) converts the RGB image I into CIE XYZ format.  The
%   RGB image I can be either of class UINT8, UINT16 or
%   DOUBLE. The output image is always of class DOUBLE.
%
%   RGB2XYZ(I,WS) uses the specified RGB working space WS. The
%   function supports the following RGB working spaces:
%
%   * `CIE'    E illuminant, gamma=2.2
%   * `Adobe'  D65 illuminant, gamma=2.2
%
%   The default workspace is CIE.
%
%   See also XYZ2RGB().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

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
