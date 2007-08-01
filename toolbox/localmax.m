% LOCALMAX  Find local maximizers
%   SEL=LOCALMAX(F) returns the indexes of the local maximizers of
%   the Q-dimensional array F.
%
%   A local maximizer is an element whose value is greater than the
%   value of all its neighbors.  The neighbors of an element i1...iQ
%   have subscripts j1...jQ such that iq-1 <= jq <= iq (excluding
%   i1...iQ itself).  For example, if Q=1 the neighbors of an element
%   are its predecessor and successor in the linear order; if Q=2, its
%   neighbors are the elements immediately to its north, south, west,
%   est, north-west, north-est, south-west and south-est
%   (8-neighborhood).
%
%   Points on the boundary of F are ignored (and never selected as
%   local maximizers).
%
%   SEL=LOCALMAX(F,THRESH) accepts an element as a mazimizer only if
%   it is not smaller thatn THRES.
%
%   SEL=LOCALMAX(F,THRESH,P) looks for neighbors only in the first P
%   dimensions of the Q-dimensional array F. This allows to process F
%   in ``slices''.
%
%   REMARK.  Matrices (2-array) with a singleton dimension are
%   interpreted as vectors (1-array). So for example LOCALMAX([0 1 0])
%   and LOCALMAX([0 1 0]') both return 2 as an aswer. However, if [0 1
%   0] is to be interpreted as a 1x2 matrix, then the correct answer
%   is the empty set, as all elements are on the boundary.
%   Unfortunately MATLAB does not distinguish between vectors and
%   2-matrices with a singleton dimension.  To forece the
%   interpretation of all matrices as 2-arrays, use
%   LOCALMAX(F,TRESH,2) (but note that in this case the result is
%   always empty!).

% AUTORIGHTS
% Copyright (C) 2006 Andrea Vedaldi
%       
% This file is part of VLUtil.
% 
% VLUtil is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software Foundation,
% Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

% TODO: Companion REFINELOCALMAX
% TODO: Do something smarter at boundaries

