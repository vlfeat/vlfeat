% VL_KMEANS  Cluster data using k-means
%   [C, A] = VL_KMEANS(X, NUMCENTERS) clusters the columns of the
%   matrix X in NUMCENTERS centers C using k-means. X may be either
%   SINGLE or DOUBLE. C has the same number of rows of X and NUMCENTER
%   columns, with one column per center. A is a UINT32 row vector
%   specifying the assignments of the data X to the NUMCENTER
%   centers.
%
%   [C, A, ENERGY] = VL_KMEANS(...) returns the energy of the solution
%   (or an upper bound for the ELKAN algorithm) as well.
%
%   KMEANS() supports different initialization and optimization
%   methods and different clustering distances. Specifically, the
%   following options are supported:
%
%   Verbose::
%     Increase the verbosity level (may be specified multiple times).
%
%   Distance::
%     Use either l1 (L1) or l2 (L2) (default) distance.
%
%   Initialization::
%     Use either random data points (RANDSEL) or k-means++ (PLUSPLUS)
%     to initialize the centers.
%
%   Algorithm:
%     Use either the standard Lloyd (LLOYD) or the accelerated
%     Elkan (ELKAN) algorithm for optimization.
%
%   Example::
%     VL_KMEANS(X, 10, 'verbose', 'distance', 'l1', 'algorithm',
%     'elkan') clusters the data point X using 10 centers, l1
%     distance, and the Elkan's algorithm.
%
%   Author:: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
