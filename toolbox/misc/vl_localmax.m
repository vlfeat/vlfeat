% VL_LOCALMAX  Find local maximizers
%   SEL=VL_LOCALMAX(F) returns the indexes of the local maximizers of
%   the Q-dimensional array F.
%
%   A local maximizer is an array element whose value is larger than
%   the value of all its neighbors.  The neighbors of an element
%   i1...iQ have subscripts j1...jQ such that iq-1 <= jq <= iq
%   (excluding i1...iQ itself).  For example, if Q=1 the neighbors of
%   an element are its predecessor and successor in the linear order;
%   if Q=2, its neighbors are the elements immediately to its north,
%   south, west, est, north-west, north-est, south-west and south-est
%   (8-neighborhood).
%
%   Points on the boundary of F are ignored (and never selected as
%   local maximizers).
%
%   SEL=VL_LOCALMAX(F,THRESH) accepts an element as a mazimizer only
%   if it is not smaller than THRESH.
%
%   SEL=VL_LOCALMAX(F,THRESH,P) looks for neighbors only in the first
%   P dimensions of the Q-dimensional array F. This allows to process
%   F in ``slices''.
%
%   Remark::
%     Matrices (2-array) with a singleton dimension are
%     interpreted as vectors (1-array). So for example VL_LOCALMAX([0 1
%     0]) and VL_LOCALMAX([0 1 0]') both return 2 as an aswer. However,
%     if [0 1 0] is to be interpreted as a 1x2 matrix, then the
%     correct answer is the empty set, as all elements are on the
%     boundary. Unfortunately MATLAB does not distinguish between
%     vectors and 2-matrices with a singleton dimension.  To forece
%     the interpretation of all matrices as 2-arrays, use
%     VL_LOCALMAX(F,TRESH,2) (but note that in this case the result is
%     always empty!).
%
%   See also: VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% TODO: Companion REFINELOCALMAX
% TODO: Do something smarter at boundaries
