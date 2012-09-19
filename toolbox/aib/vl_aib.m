% VL_AIB  Agglomerative Information Bottleneck
%   PARENTS = VL_AIB(PCX) runs Agglomerative Information Bottleneck
%   (AIB) on the class-feature co-occurrence matrix PCX and returns a
%   vector PARENTS representing the sequence of compressed AIB
%   alphabets.
%
%   PCX is the joint probability of the occurrence of the class label
%   C and the feature value X. PCX has one row for each class label
%   and one column for each feature value, non negative entires and
%   sums to one. AIB iteratively merges the pair of feature values
%   that decreases the mutual information I(X,C) the least. This
%   compresses the alphabet of the discrete random variable X in such
%   a way that the new variable is still informative about C.
%
%   Merge operations are represented by a binary tree. The nodes of
%   the tree correspond to the original feature values and any other
%   value obtained by merging.
%
%   The vector PARENTS represents the merge tree. The nodes are
%   numbered in breadth-first order, starting from the leaves. The
%   numbers associated to the tree leaves correspond to the original
%   feature values (so the first leaf has number one and correspond to
%   the first feature value).  In total there are 2*M-1 nodes, where M
%   is the number of feature values (the number of columns of
%   PCX). The internal nodes are numbered according to the order in
%   which AIB generates them. It is therefore possible to recover from
%   the tree the state of the AIB algorithm at each step (see also
%   VL_AIBCUT()). PARENTS is a UINT32 array with one element for each
%   tree node storing the index of the parent node. The root parent is
%   conventionally set to 1.
%
%   Feature values with null probability (null columns of the PCX
%   matrix) are ignored by the AIB algorithm and the corresponding
%   entries in the PARENTS vectors are set to zero. Notice that this
%   causes the root of the tree to have index smaller of 2*M-1
%   (PARENTS has still 2*M-1 entries, but the last portion is
%   zero-padded).
%
%   Alternatively, the option ClusterNull can be used to assign the
%   null probability values to a special value. The result is similar
%   to pretending that the null probability nodes have indeed very
%   small probability, uniform across categories.
%
%   [PARENTS, COST] = VL_AIB(...) returns the values COST of the cost
%   function being optimized by AIB (i.e. the mutual information
%   I(X,C)). COST has M column. The first column is the initial value
%   of the cost function. The others correspond to the cost after each
%   of the M-1 merges. If less than M-1 merges are performed, the rest
%   of the vector is filled with NaNs.
%
%   VL_AIB() accepts the following options:
%
%   Verbose::
%     If specified, increase verbosity level.
%
%   ClusterNull::
%     If specified, do not signal null nodes; instead cluster them.
%
%   See also: VL_AIBCUT(), VL_AIBHIST(), VL_AIBCUTHIST(),
%   VL_AIBCUTPUSH(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
