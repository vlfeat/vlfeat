% VL_BINSUM  Binned summation
%   H = VL_BINSUM(H,X,B) adds the elements of the array X to the
%   elements of the array H indexed by B. X and B must have the same
%   dimensions, and the elements of B must be valid indexes for the
%   array H (except for null indexes, which are silently skipped). An
%   application is the calculation of a histogram H, where B are the
%   occurences and X are the occurence weights.
%
%   H = VL_BINSUM(H,X,B,DIM) operates only along the specified
%   dimension DIM. In this case, H, X and B are array of the same
%   dimensions, except for the dimension DIM of H, which may differ,
%   and B is an array of subscripts of the DIM-th dimension of H. A
%   typical application is the calculation of multiple histograms,
%   where each histogram is a 1-dimensional slice of the array H along
%   the dimension DIM.
%
%   X can also be a scalar. In this case, the value X is summed to all
%   the specified bins.
%
%   Example::
%     The following relations illustrate VL_BINSUM() behavior:
%       VL_BINSUM([0 0],  1, 2) = [0 1]
%       VL_BINSUM([1 7], -1, 1) = [0 7]
%       VL_BINSUM(EYE(3), [1 1 1], [1 2 3], 1) = 2*EYE(3)
%
%   See also: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
