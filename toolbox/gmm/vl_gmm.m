% Vl_GMM  Learn a Gaussian Mixture Model using EM
%   [MEANS, SIGMAS, WEIGHTS] = VL_GMM(X, NUMCLUSTERS) fits a GMM with
%   NUMCLUSTERS components to the data X. Each column of X represent a
%   sample point. X may be either SINGLE or DOUBLE. MEANS, SIGMAS, and
%   WEIGHTS are repsectively the means, the diagonal covariances, and
%   the prior probabilities of the Guassian modes. MEANS and SIGMAS
%   have the same number of rows as X and NUMCLUSTERS columns with one
%   column per mode. WEIGHTS is a row vector with NUMCLUSTER entries
%   summing to one.
%
%   [MEANS, SIGMAS, WEIGHTS, LL] = VL_GMM(...) returns the
%   loglikelihood (LL) of the model as well.
%
%   [MEANS, SIGMAS, WEIGHTS, LL, POSTERIORS] = VL_GMM(...) returns the
%   loglikelihood of the solution and posterior probabilities of the
%   Gaussian modes given each data point. The POSTERIORS matrix has
%   NUMCLUSTERS rows and NUMDATA columns.
%
%   VL_GMM() supports different initialization and optimization
%   methods. Specifically, the following options are supported:
%
%   Verbose::
%     Increase the verbosity level (may be specified multiple times).
%
%   Initialization:: RAND
%     RAND initializes the means as random data poitns and the
%     covaraince matrices as the covariance of X. CUSTOM allow
%     specifying the initial means, covariances, and prior
%     probabilities.
%
%   InitMeans:: []
%    Specify the initial means (size(X,1)-by-NUMCLUSTERS matrix).
%
%   InitWeights:: []
%    Specify the initial weights (a vector of dimension NUMCLUSTER).
%
%   InitSigmas:: []
%    Specify the initial diagonal covariance matrices
%
%   Multithreading:: [SERIAL]
%     Choose whether to use SERIAL or PARALLEL (multi-core)
%     computations.
%
%   NumRepetitions:: 1
%     Number of times to restart EM. The solution with maximum
%     loglikelihood is returned.
%
%   SigmaBound:: 10e-6
%     Set the lower bound on the covariance diagonal entries. This
%     is particularly important if the data contains singleton
%     dimensions, and generally useful for stability.
%
%   Example::
%     VL_GMM(X, 10, 'verbose', 'multithreading', 'parallel',
%     'MaxNumIterations', 20) estimates the mixture of 10 gaussians
%     using at mosst 20 iterations.
%
%   See also: VL_KMEANS(), VL_HELP().

% Authors: David Novotny and Andrea Vedaldi

% Copyright (C) 2013 David Novotny and Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
