% ALLDIST2  Pairwise distances
%  D = ALLDIST2(X,Y) returns the pairwise distance matrix D of the
%  columns of S1 and S2, yielding
%
%    D(i,j) = || X(:,i) - Y(:,j) ||_2^2 for all i,j
%
%  where || . ||_2^2 denotes the squared L2 norm.
%
%  ALLDIST2(X) returns the pairwise distance matrix fo the columns of
%  S, yielding
%
%    D(i,j) = || X(:,i) - X(:,j) ) ||_2^2 for all i,j.
%
%  ALLDIST2(...,'<norm>') changes the computed norm. Supported values
%  for <norm> are
%
%  <norm> norm type    D(i,j)
%  ------------------------------------------
%    l0   l0 norm      sum_k (Xki \not= Ykj)
%    l1   l1 norm      sum_k |Xki - Ykj|
%    l2   l2 norm      sum_k (Xki - Ykj)^2
%    linf l_inf norm   max_k |Xki - Xkj|
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
