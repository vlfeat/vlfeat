function J=rgb2xyz(I,ws)
% RGB2XYZ  Color space conversion
%   J=RGB2XYZ(I) converts the RGB image I into CIE XYZ format.  The
%   RGB image I can be either of class 'uint8', 'uint16' or
%   'double'. The output image is always of class 'double'
%
%   RGB2XYZ(I,WS) uses the specified RGB working space WS. The
%   function supports the following RGB working spaces:
%
%   * `CIE'    E illuminant, gamma=2.2
%   * `Adobe'  D65 illuminant, gamma=2.2
%
%   The default workspace is CIE.
%
%   See also XYZ2RGB(), RGB2DOUBLE().

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

[M,N,K] = size(I) ;

if K~=3
	error('I must be a MxNx3 array.') ;
end

I=rgb2double(I) ;

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


