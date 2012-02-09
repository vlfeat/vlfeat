function h = vl_ikmeanshist(K,asgn)
% VL_IKMEANSHIST  Compute histogram of quantized data
%  H = VL_IKMEANSHIST(K,ASGN) computes the histogram of the IKM clusters
%  activated by cluster assignments ASGN.
%
%  See also: VL_IKMEANS(), VL_IKMEANSPUSH(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

h = zeros(K,1) ;
h = vl_binsum(h, 1, double(asgn)) ;
