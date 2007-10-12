function h = ikmeanshist(K,asgn)
% IKMEANSHIST  Compute histogram of quantized data
%  H = IKMEANSHIST(K,ASGN) computes the histogram of the IKM clusters
%  activated by cluster assignments ASGN.
%
%  See also IKMEANSPUSH().

% AUTORIGHTS

h = zeros(K,1) ;
h = binsum(h, 1, double(asgn)) ;
