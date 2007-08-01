function varargout = affine(A,T,varargin)
% AFFINE  Apply affine transformation to points
%  Y = AFFINE(A,T,X) applies the affine transformatio (A,T) to points
%  X. X contains one point per column.
%
%  [Y1,Y2,...] = AFFINE(A,T,X1,X2,...) applies the affine
%  transformations to points (X1,X2,...). Arrays X1,X2,... contain one
%  of the coordinates of the points each.

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

if numel(varargin)==1

  X = varargin{1} ;
  
  [n,k] = size(X);
  if n == 2
    Y(1,:) = A(1,1)*X(1,:) + A(1,2)*X(2,:) + T(1) ;
    Y(2,:) = A(2,1)*X(1,:) + A(2,2)*X(2,:) + T(2) ;
  elseif n == 3 
    Y(1,:) = A(1,1)*X(1,:) + A(1,2)*X(2,:) + A(1,3) * X(3,:) + T(1) ;
    Y(2,:) = A(2,1)*X(1,:) + A(2,2)*X(2,:) + A(2,3) * X(3,:) + T(2) ;
    Y(3,:) = A(3,1)*X(1,:) + A(3,2)*X(2,:) + A(3,3) * X(3,:) + T(3) ;
  else
    Y = A*X + repmat(T,1,k) ;
  end
  
  varargout{1} = Y ;
  
else
  
  n = numel(varargin) ;

  if n == 2
    varargout{1} = A(1,1)*varargin{1} + A(1,2)*varargin{2} + T(1) ;
    varargout{2} = A(2,1)*varargin{1} + A(2,2)*varargin{2} + T(2) ;
  elseif n == 3 
    varargout{1} = A(1,1)*varargin{1} + A(1,2)*varargin{2} + A(1,3)*varargin{3} + T(1) ;
    varargout{2} = A(2,1)*varargin{1} + A(2,2)*varargin{2} + A(2,3)*varargin{3} + T(2) ;
    varargout{3} = A(3,1)*varargin{1} + A(3,2)*varargin{2} + A(3,3)*varargin{3} + T(3) ;
  else
    for i=1:n
      varargout{i} = T(i) * ones(size(varargin{1})) ;
      for j=1:n
        varargout{i} = varargout{i} + A(i,j)*varargin{j} ;
      end
    end
  end
end
