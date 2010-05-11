function h = vl_hikmeanshist(tree,path)
% VL_HIKMEANSHIST  Compute histogram of quantized data
%  H = VL_HIKMEANSHIST(TREE,PATH) computes the histogram of the HIKM tree
%  nodes activated by the root-to-leaf paths PATH. PATH is usually
%  obtained by quantizing data by means of VL_HIKMEANSPUSH().
%
%  The histogram H has one bin for each node of the HIKM tree TREE.
%  The tree has K = TREE.K nodes and depth D = TREE.DEPTH.  Therefore
%  there are M = (K^(D+1) - 1) / (K - 1) nodes in the tree (not
%  counting the root which carries no information). Nodes are stacked
%  into a vector of bins in breadth first order.
%
%  Example::
%    The following relations illustrate the structure of PATH:
%      H(1)   = # of paths such that PATH(1,:) = 1
%      H(K)   = # of paths such that PATH(1,:) = K
%      H(K+1) = # of paths such that PATH(1:2,:) = [1 ; 1]
%      H(K+K) = # of paths such that PATH(1:2,:) = [1 ; K]
%
%  See also:: VL_HIKMEANS(), VL_HIKMEANSPUSH(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

% PATH(:,k) is colum of subscripts i1 i2 ... id identifying a path in
% the tree. In a bread first enumeration of the tree nodes (starting
% from one and not counting the root), the node of subscripts
% i1,i2,...id has index
%
% idx = i1 K^{d-1} + i2 K^{d-2} + ... + id
%
% where we assumed the indeces i1,i2,... start from 1. This formula
% can be easily computed recursively. Since we also have a root
% node, we need to add one.

K = tree.K ;
D = tree.depth ;
M = (K^(D+1) - 1) / (K - 1) ;

h = zeros(M, 1) ;
p = zeros(1,size(path,2)) ;

h(1) = size(path,2) ;

for d=1:D
  p = p * K + double(path(d,:))  ;
  h = vl_binsum(h, 1, p + 1) ;
end
