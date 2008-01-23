function h = ikmeanshist(K,asgn)
% IKMEANSHIST  Compute histogram of quantized data
%  H = IKMEANSHIST(K,ASGN) computes the histogram of the IKM clusters
%  activated by cluster assignments ASGN.
%
%  See also IKMEANSPUSH().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

h = zeros(K,1) ;
h = binsum(h, 1, double(asgn)) ;
