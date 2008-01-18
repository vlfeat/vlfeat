% AIB  Agglomerative Information Bottleneck
%   PARENTS = AIB(PCX) runs Agglomerative Information Bottleneck (AIB)
%   on the category-feature co-occurence matrix PCX and returns a
%   vector PARENTS representing all the fetures join operations.
%
%   The function iteratively merges the two vlaues of the feature X
%   that casuses the smallest decrease in mutual information between
%   the random variables X and C.
%
%   Merge operations are arranged in a binary tree. The nodes of the
%   tree correspond to the original feature values and any other value
%   obtained as a result of a merge operation. The nodes are numbered
%   in breadth-first order, starting from the leaves. The first node
%   has number one. The numbers associated to the leaves correspond to
%   the original feature values.  In total there are 2*M-1 nodes,
%   where M is the number of values (number of columns of PCX). The
%   internal nodes order reflects the order of the AIB merge
%   operations. It is therefore possible to recover from the tree the
%   state of the AIB algorithm at each step (see AIBCUT(),
%   AIBFINDCUT()).
%
%   The function returns a UINT32 array with one element per tree
%   node. Each element is the index of the parent node. The root
%   parent is equal to 1.
%
%   Feature values with null probability are ignored by the AIB
%   algorithm. By default these are exlcuded from the caluclation and
%   their parent is set to zero. Notice that this causes the root of
%   the tree to have index smaller of 2*M-1 (the returned vector is
%   still 2*M-1 elements lenght, but the last portion is
%   zero-padded).
%
%   Alternatively, the option ClusterNull can be used to cluster the
%   null nodes in its own cluster. In this case, the results is as if
%   pretenting that the null node have very small probability, uniform
%   across categories.
%
%   [PARENTS, COST] = AIB(...) returns the values cost of the COST
%   function being optimizied, one for each node in PARENTS. COST has
%   M column. The first column is the inital value of the cost
%   function. The others correspond to the cost after each of the
%   M-1 merges. If less than M-1 merges are performed, the rest of
%   the vector is filled with NaNs.
%
%   Options:
%
%   Verbose::
%     Increase verbosity level.
%
%   CLusterNull::
%     Do not signal null nodes; instead cluster them.
%
%   See also AIBCUT().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
