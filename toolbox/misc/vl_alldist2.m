% VL_ALLDIST2  Pairwise distances
%  D = VL_ALLDIST2(X,Y) returns the pairwise distance matrix D of the
%  columns of S1 and S2, yielding
%
%    D(i,j) = sum (X(:,i) - Y(:,j)).^2
%
%  VL_ALLDIST2(X) returns the pairwise distance matrix fo the columns of
%  S, yielding
%
%    D(i,j) = sum (X(:,i) - X(:,j)).^2
%
%  VL_ALLDIST2(...,'METRIC') changes the computed distance. Supported
%  values for METRIC are
%
%   METRIC  D(i,j)
%   --------------------------------------------------------
%    LINF   max |X  - Y|
%    L2     sum (X  - Y).^2
%    L1     sum |X  - Y|
%    L0     sum (X ~= Y)
%    CHI2   sum (X  - Y).^2 ./ (X + Y)
%    HELL   sum (X^.5 - Y^.5) .^ 2
%
%  (Notice that the standard definition of chi2 is half of what is
%  computed here).
%
%  VL_ALLDIST2(...,'KERNEL') computes the following 'kernels' K:
%
%   KERNEL  K(i,j)
%   ---------------------------------------------------------
%    KL2    sum X .* Y
%    KL1    sum min (X, Y)
%    KCHI2  2 * sum (X .* Y) ./ (X + Y)
%    KHELL  (X .* Y) .^ 0.5
%
%  The constant are chosen so that D(i,j) = K(i,i) + K(j,j) - 2 K(i,j)
%  where D is the metric corresponding to the kenrel (if the arguments
%  are non-negative vectors). Each kernel can be interpreted as the
%  inner product inducing the corresponding metric in an embedding of
%  the real space into an approrpiate reproducing Kenrel Hilbert
%  space.
%
%  VL_ALLDIST2() supports several storage classes. X and Y must have the
%  same storage class. The sotrage class of D is promoted to reduce
%  the chance of overvlow, but this is not checked.
%
%    X & Y class      D class  
%   ---------------------------
%    UINT8            UINT32   
%     INT8             INT32   
%    UINT16           UINT32   
%     INT16            INT32   
%    UINT32           UINT32   
%     INT32            INT32   
%    SINGLE           SINGLE   
%    DOUBLE           DOUBLE   
%
%  Warning: Both chi2 and kchi2 use integer math when presented with
%  integer data types. This can easily result in zeros where you did
%  not expect them.
%
%  See also:: VL_HELP().


% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
