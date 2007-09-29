% BINSUM  Binned summation
%  H=BINSUM(H,X,B) adds the elements of X to the elements of H
%  indicated by B. X and B are arrays of the same dimensions, and the
%  elements of B must be valid indexes for the array H.
%
%  H=BINSUM(H,X,B,DIM) operates only on the specified dimension
%  DIM. In this case, H, X and B are array of the same dimensions,
%  except for the dimension DIM of H which may differ. The indexes B
%  are subscript for the dimension DIM of H.
%
%  This function is very useful to compute histograms.
%
%  Examples: BINSUM([0 0],  1, 2) = [0 1]
%            BINSUM([1 7], -1, 1) = [0 7]
%            BINSUM([0 0; 0 0], [1 1], [1 2], 1) = [1 0; 0 1]

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
