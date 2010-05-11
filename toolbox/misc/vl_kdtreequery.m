% VL_KDTREEQUERY Query KD-tree
%   [INDEX, DIST] = VL_KDTREEQUERY(KDTREE, X, Y) computes the nearest
%   column of X to each column of Y (in Euclidean distance). KDTREE is
%   a forest of kd-trees build by VL_KDTREEBUILD(). X is a
%   NUMDIMENSIONS x NUMDATA data matrix of class SINGLE or DOUBLE with
%   the data indexed by the kd-trees (it must be the same data matrix
%   passed to VK_KDTREEBUILD() to build the trees). Y is the
%   NUMDIMENSIONS x NUMQUERIES matrix of query points and must have
%   the same class of X. INDEX is a 1 x NUMQUERIES matrix of class
%   UINT32 with the index of the nearest column of X for each column
%   of Y. DIST is a 1 x NUMQUERIES vector of class SINGLE or DOUBLE
%   (depending on the class of X and Y) with the corresponding squared
%   Euclidean distances.
%
%   [INDEX, DIST] = VL_KDTREEQUERY(..., 'NUMNEIGHBORS', NN) can be
%   used to return the N nearest neighbors rather than just the
%   nearest one. In this case INDEX and DIST are NN x NUMQUERIES
%   matrices. Neighbors are returned by increasing distance.
%
%   VL_KDTREEQUERY(..., 'MAXCOMPARISONS', NCOMP) performs at most
%   NCOMP comparisons for each query point. In this case the result is
%   only approximate (i.e. approximated nearest-neighbors, or ANNs)
%   but the speed can be greatly improved.
%
%   Options:
%
%   NumNeighbors::
%     Sets the number of neighbors to compute for each query point (by
%     default 1).
%
%   MaxComparisons:
%     Sets the maximum number of comparisons per query point. The
%     special value 0 means unbounded. The default is 0.
%
%   See also:: VL_KDTREEBUILD()

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
