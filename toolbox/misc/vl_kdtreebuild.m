% VL_KDTREEBUILD  Build randomized kd-tree
%   FOREST = VL_KDTREEBUILD(X) returns a structure FOREST containing
%   the kd-tree indexing the data X. X is a M x N dimensional matrix
%   of class DOUBLE or SINGLE with one data point per column. Use
%   VL_KDTREEQUERY() to query the index.
%
%   VL_KDETREEBUILD() accepts the following options:
%
%   NumTrees:: 1
%     Specifies the number of semi-randomized KD-trees to add to the
%     forest. Multiple trees can be used to improve approximate
%     nearest-neighbors searches with VL_KDTREEQUERY().
%
%   ThresholdMethod:: MEDIAN
%     Specifies the thresholding method used to split the data.  Two
%     methods are supported: split around the median (MEDIAN) or split
%     around the mean (MEAN).
%
%   Verbose::
%     Increases the verbosity level (can be repeated).
%
%   The FOREST structure has the following fields:
%
%   FOREST.TREES::
%     A structure array with one element per tree.
%
%   FOREST.DIMENSION::
%     Dimensionality of the indexed data.
%
%   FOREST.NUMDATA::
%     Number of indexed data points.
%
%   The TREE structure has the following fields:
%
%   TREE.NODES::
%      A structure array representing the nodes of the tree.
%
%   TREE.DATAINDEX::
%      A 1 x NUMDATA vector of class UINT32 representing a permutation
%      of the data.
%
%   Nodes are numbered from 1 to NUMNODES.  The NODES structure array
%   has the following fields:
%
%   NODES.LOWERCHILD and NODES.UPPERCHILD::
%      1 x NUMNODES vectors of class INT32. A positive value is the
%      index of the lower/upper child node. A negative value denotes a
%      leaf and is (after negation) is the first or last element plus
%      one of a range of entries in the permutation TREE.DATAINDEX.
%      Such entries are in turn indexes of the data points that belong
%      to that leaf. Typically there is one point per leaf.
%
%   NODES.SPLITDIMENSION and NODES.SPLITTHRESHOLD::
%      1 x NUMNODES vector of class UINT32 and DOUBLE, respectively,
%      with the index of the splitting dimension and the threshold for
%      each node.
%
%   See also: VL_KDTREEQUERY()

% Authors: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
