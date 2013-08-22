% VL_IKMEANS  Integer K-means
%   C = VL_IKMEANS(X,K) returns the centers of a K-means paritioning of
%   the data space X. X must be of class UINT8. C is of class UINT32.
%
%   [C, I] = VL_IKMEANS(...) returns the cluster associations I of the
%   data as well.
%
%   VL_IKMEANS() accepts the following options:
%
%   MaxIters:: 200
%     Maximum number of iterations before giving up (the algorithm
%     stops as soon as there is no change in the data to cluster
%     associations).
%
%   Method:: Lloyd
%     Algorithm to use ('Lloyd', 'Elkan').
%
%   Verbose::
%     Increase the verbosity level.
%
%  See also: VL_IKMEANSPUSH(), VL_IKMEANSHIST(), VL_HIKMEANS(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
