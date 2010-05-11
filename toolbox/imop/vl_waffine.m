function varargout = vl_waffine(A,T,varargin)
% VL_WAFFINE  Apply affine transformation to points
%  Y = VL_WAFFINE(A,T,X) applies the affine transformatio (A,T) to points
%  X. X contains one point per column.
%
%  [Y1,Y2,...] = VL_WAFFINE(A,T,X1,X2,...) applies the affine
%  transformation (A,T) to the points (X1,X2,...). Each array
%  X1,X2,... contains one of the coordinates of the points.
%
%  See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

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
