function h = vl_ikmeanshist(K,asgn)
% VL_IKMEANSHIST  Compute histogram of quantized data
%  H = VL_IKMEANSHIST(K,ASGN) computes the histogram of the IKM clusters
%  activated by cluster assignments ASGN.
%
%  See also:: VL_IKMEANS(), VL_IKMEANSPUSH(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

h = zeros(K,1) ;
h = vl_binsum(h, 1, double(asgn)) ;
