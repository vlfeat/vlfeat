% ALLDIST2  Pairwise distances
%  D = ALLDIST2(X,Y) returns the pairwise distance matrix D of the
%  columns of S1 and S2, yielding
%
%    D(i,j) = sum (X(:,i) - Y(:,j)).^2
%
%  ALLDIST2(X) returns the pairwise distance matrix fo the columns of
%  S, yielding
%
%    D(i,j) = sum (X(:,i) - X(:,j)).^2
%
%  ALLDIST2(...,'<norm>') changes the computed distance. Supported values
%  for <dist> are
%
%  <dist>  D(i,j)
%  --------------------------------------------------------
%   l0     sum (X(:,i) ~= Y(:,j))
%   l1     sum |X(:,i)  - Y(:,j)|
%   l2     sum (X(:,i)  - Y(:,j)).^2
%   linf   max |X(:,i)  - X(:,j)|
%   chi2   sum (X(:,i) - Y(:,j)).^2 ./ (X(:,i) + Y(:,j))
%
%  (Notice that the standard definition of chi2 is half of what is
%  computed here).
%
%  ALLDIST2(...,'<ker>') computes the following 'kernels' K:
%
%  <ker>   K(i,j)
%  ---------------------------------------------------------
%   kl1    sum (min(X(:,i),Y(:,j)))
%   kchi2  2 * sum (X(:,i) .* Y(:,j) ./ (X(:,i) + Y(:,j)))
%
%  The constant are chosen so that D(i,j) = K(i,i) + K(j,j) - 2 K(i,j)
%  where D is the distance corresponding to the kenrel (if the
%  arguments are non-negative vectors).
%
%  ALLDIST2() supports directly most data types. X and Y must have the
%  same storage class. The sotrage class of D is promoted to reduce
%  the chance of overvlow, but no check is perfomed to prevent this
%  from happening (saturation math is NOT used).
%
%   X & Y class      D class  
%  ---------------------------
%   UINT8            UINT32   
%    INT8             INT32   
%   UINT16           UINT32   
%    INT16            INT32   
%   UINT32           UINT32   
%    INT32            INT32   
%   SINGLE           SINGLE   
%   DOUBLE           DOUBLE   

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
