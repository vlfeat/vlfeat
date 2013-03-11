% VL_GMM  Cluster data using expectation maximization GMM
%   [MEANS, SIGMAS, WEIGHTS] = VL_GMM(X, NUMCLUSTERS) estimates the
%   gaussian mixture model of the data points stored in the columns of the
%   matrix X in NUMCLUSTERS gaussians using EM algorithm. X may be either
%   SINGLE or DOUBLE. MEANS and SIGMAS has the same number of rows as X and
%   NUMCLUSTERS columns with one column per estimated gaussian.
%   Note that the algorithm estimates the gaussians with diagonal sigma
%   matrices. SIGMAS contains these diagonals in each column.
%   WEIGHTS cointains individual weights of estimated gaussians in a 
%   NUMCLUSTERS long one row vector.
%
%   [MEANS, SIGMAS, WEIGHTS, LL] = VL_GMM(...) returns the loglikelyhood 
%   (LL) of the solution as well.
%
%   [MEANS, SIGMAS, WEIGHTS, LL, POSTERIORS] = VL_GMM(...) returns the 
%   loglikelyhood of the solution and posterior probabilities of the 
%   correspondeces of individual data points to specific clusters. 
%   POSTERIORS matrix has NUMCLUSTERS rows and NUMDATA columns.
%   Each column contains the set of posteriors of a data point to a
%   specific gaussian.
%
%   VL_GMM() supports different initialization and optimization
%   methods. Specifically, the following options are supported:
%
%   Verbose::
%     Increase the verbosity level (may be specified multiple times).
%
%   Initialization:: [RAND]
%     Use either random data points as initial means, covariance of the 
%     whole data as initial diagonals of covariance matrices (RAND) or 
%     custom initialization of starting sigmas and means of the mixture 
%     (CUSTOM).
%
%   InitMeans::
%    Specify the initial means (size(X,1)-by-numClusters matrix) 
%    of the gaussian mixture.
%
%   InitWeights::
%    Specify the initial weights (numClusters sized vector) 
%    of the gaussian mixture.
%
%   InitSigmas::
%    Specify the initial diagonals of covariance matrices
%    (size(X,1)-by-numClusters matrix) of the gaussian mixture.
%
%   Multithreading:: [SERIAL]
%     Choose whether you want to use PARALLEL or SERIAL approach to the 
%     computation of the expectation and maximization steps.
%
%   NumRepetitions:: [1]
%     Number of times to restart EM. The solution with maximal
%     loglikelyhood is returned.
%
%   SigmaBound:: [10e-6]
%     Set the lower bound on sigma diagonals. Suitable when data contains
%     singleton dimensions.
%
%   Example::
%     VL_GMM(X, 10, 'verbose', 'multithreading', 'parallel', 'MaxNumIterations', 20) 
%     estimates the mixture of 10 gaussians in 20 iterations at maximum.
%
%   See also: VL_HELP().

% Authors: Andrea Vedaldi, David Novotny

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
