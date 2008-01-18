% IKMEANS  Integer K-means
%   C = IKMEANS(X,K) returns the centers of a K-menas paritioning of
%   the data space X. X must be of class UINT8. C is of class UINT32.
%
%   [C, I] = IKMEANS(...) returns the cluster associations I of the
%   data as well.
%
%   IKMEANS(...,options...) accepts the following options
%
%   MaxPasses [200]::
%     Maximum number of iterations before giving up (the algorithm
%     stops as soon as there is no change in the data to cluster
%     associations).
%
%   Verbose::
%     Increase the verbosity level.

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
