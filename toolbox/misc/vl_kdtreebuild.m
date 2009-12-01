% VL_KDTREEBUILD  Build randomized KD-tree
%   FOREST = VL_KDTREEBUILD(X) builds a forest FORES containing one
%   KD-tree indexing the data X. X is a M x N dimensional matrix of
%   class SINGLE with one data point per column.
%
%   VL_KDTREEBUILD(X, 'NumTrees', T) builds a forest of T randomized
%   trees.
%
%   FOREST is a structure with the following fields:
%
%   FOREST.TREES::
%     Struct-array with one elment per tree.
%
%   FOREST.NUMDIMENSIONS::
%     Dimensionality of the indexed data.
%
%   FOREST.NUMDATA::
%     Number of indexed data points.
%
%  Each TREE structure has the following fields:
%
%  TREE.NODES::
%     A structure representing the tree nodes.
%
%  TREE.DATAINDEX::
%     A 1 x NUMDATA vector of class UINT32 representing a permutation
%     of the data.
%
%  Nodes are numbered from 1 to NUMNODES..  The NODES structure has
%  the following fields:
%
%  NODES.LOWERCHILD and NODES.UPPERCHILD::
%     1 x NUMNODES vectors of class INT32. A positive value is the
%     index of the lower/upper child node. A negative value denotes a
%     leaf and is (after negation) is the first or last element plus
%     one of a range of entries in the permutation TREE.DATAINDEX.
%     Such entries are in turn indexes of the data points that belong
%     to that leaf. Typically there is one point per leaf.
%
%  NODES.SPLITDIMENSION and NODES.SPLITTHRESHOLD::
%     1 x NUMNODES vector of class UINT32/SINGLE with the index of the
%     splitting dimension and the threshold for each node.
%
%   Options:
%
%   NumTrees::
%     Specify the number of trees to build.
%
%   ThresholdMethod::
%     One of 'mean' or 'median'. Default is 'mean'.
%
%   Verbose::
%     Be verbose.

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
