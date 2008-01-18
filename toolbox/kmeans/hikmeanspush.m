% HIKMEANSPUSH   Push data down an integer K-means tree
%   PATH = HIKMEANSPUSH(TREE,X) quanizes the data X with the
%   hierachical integer K-means tree TREE. This is obtained by
%   calculating the path of each datum from the root of TREE down to a
%   leaf. Here X has a datum for each column and each column of PATH
%   represents the corresponding root-to-leaf path.
%
%   Example: PATH(:,1) = [2 1 4] means that the first datum follows
%   the branches number 2, 1 and 4 starting from the root of
%   the HIKM tree ending to a leaf.
%
%   See also HIKMEANS().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

