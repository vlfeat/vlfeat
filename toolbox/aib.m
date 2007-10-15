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
%   where M is the number of values (number of columns of PCX).
%
%   The function returns a UINT32 array whit one element per tree
%   node. Each element is the number of the parent node. The root
%   parent is equal to 1.
%
%   Feature values with null probability are ignored by the algorithm
%   and their nodes have parents set to 0. In this case, there are
%   less than M-1 merges so the last portion of the PARENTS vector is
%   also set to 0.
%
%   [PARENTS, COST] = AIB(...) returns the values cost of the COST
%   function being optimizied, one for each node in PARENTS. COST has
%   M column. The first column is the inital value of the cost
%   function. The others correspond to the cost after each of the
%   M-1 merges. If less than M-1 merges are performed, the rest of
%   the vector is filled with NaNs.
%
%   See also AIBCUT().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
