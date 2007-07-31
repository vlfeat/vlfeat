function J=xyz2rgb(I,ws)
% XYZ2RGB  Convert XYZ to RGB
%   J = XYZ2RGB(I) the XYZ image I in RGB format.
%
%   XYZ2RGB(I,WS) uses the specified RGB working space WS. The function
%   supports the following RGB working spaces:
%
%   - `CIE'    E illuminant, gamma=2.2
%   - `Adobe'  D65 illuminant, gamma=2.2
%
%   The default is CIE.
%
%   See also RGB2XYZ().

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
