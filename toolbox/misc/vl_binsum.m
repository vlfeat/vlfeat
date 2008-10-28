% VL_BINSUM  Binned summation
%  H = VL_BINSUM(H,X,B) adds the elements of X to the elements of H
%  indicated by B. X and B are arrays of the same dimensions, and the
%  elements of B must be valid indexes for the array H.  This call can
%  be used to compute histograms.
%
%  H = VL_BINSUM(H,X,B,DIM) operates only along the specified dimension
%  DIM. In this case, H, X and B are array of the same dimensions,
%  except for the dimension DIM of H, which may differ, and B is an
%  array of subscripts of the DIM-th dimension of H. This call can be
%  used to compute multiple histograms at once, with one histogam for
%  each 1-dimensional slice of H along dimension DIM.
%
%  H = VL_BINSUM(H,X,...) where X is a scalar sums the same values to all
%  elements.
%
%  Example::
%    The following relations illustrate VL_BINSUM() behavior:
%      VL_BINSUM([0 0],  1, 2) = [0 1]
%      VL_BINSUM([1 7], -1, 1) = [0 7]
%      VL_BINSUM(EYE(3), [1 1 1], [1 2 3], 1) = 2*EYE(3)
%
%  See also:: VL_HELP_VLFEAT().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
