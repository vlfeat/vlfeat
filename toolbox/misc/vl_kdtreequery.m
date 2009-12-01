% VL_KDTREEQUERY Query KD-tree
%   [INDEX, DIST] = VL_KDTREEQUERY(KDTREE, X, Y) computes the nearest
%   column of X to each column of Y (in Euclidean distance). KDTREE is
%   a KD-tree (or forest) build by VL_KDTREEBUILD(). X is a
%   NUMDIMENSIONS x NUMDATA matrix of class SINGLE with the indexed
%   data (it must be the same matrix passed to VK_KDTREEBUILD()). Y is
%   the NUMDIMENSIONS x NUMQUERIES corresponding matrix of query data
%   points.  INDEX is a 1 x NUMQUERIES matrix of class UINT32 with the
%   index of the nearest column of X for each column of Y. DIST is a 1
%   x NUMQUERIES vector with the corresponding squared Euclidean
%   distances.
%
%   [INDEX, DIST] = VL_KDTREEQUERY(..., 'NUMNEIGHBORS', NN) can be
%   used to return the N nearest neighbors. In this case INDEX and
%   DIST are NN x NUMQUERIES matrices. Neighbors are returned by
%   increasing distance.
%
%   VL_KDTREQUERY(..., 'MAXCOMPARISONS', NCOMP) performs at most NCOMP
%   comparisons for each query point. In this case the result is only
%   approximate (ANN) but the speed can be greatly improved.
%
%   Options:
%
%   NumNeihgbors::
%     Number of neighbors to compute (by default 1).
%
%   MaxVisits::
%     Maximum number of comparisons per point (by default 0, which
%     means unbounded comparisons).
%
%   See also:: VL_KDTREEBUILD()

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
