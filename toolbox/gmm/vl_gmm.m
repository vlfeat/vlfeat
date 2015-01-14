% Vl_GMM  Learn a Gaussian Mixture Model using EM
%   [MEANS, COVARIANCES, PRIORS] = VL_GMM(X, NUMCLUSTERS) fits a GMM with
%   NUMCLUSTERS components to the data X. Each column of X represent a
%   sample point. X may be either SINGLE or DOUBLE. MEANS, COVARIANCES, and
%   PRIORS are respectively the means, the diagonal covariances, and
%   the prior probabilities of the Guassian modes. MEANS and COVARIANCES
%   have the same number of rows as X and NUMCLUSTERS columns with one
%   column per mode. PRIORS is a row vector with NUMCLUSTER entries
%   summing to one.
%
%   [MEANS, COVARIANCES, PRIORS, LL] = VL_GMM(...) returns the
%   loglikelihood (LL) of the model as well.
%
%   [MEANS, COVARIANCES, PRIORS, LL, POSTERIORS] = VL_GMM(...) returns
%   the posterior probabilities POSTERIORS of the Gaussian modes given
%   each data point. The POSTERIORS matrix has NUMCLUSTERS rows and
%   NUMDATA columns.
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
%   InitMeans:: none
%     Specify the initial means (size(X,1)-by-NUMCLUSTERS matrix).
%
%   InitPriors:: none
%     Specify the initial weights (a vector of dimension NUMCLUSTER).
%
%   InitCovariances:: none
%     Specify the initial diagonal covariance matrices
%
%   NumRepetitions:: 1
%     Number of times to restart EM. The solution with maximum
%     loglikelihood is returned.
%
%   CovarianceBound:: 10e-6
%     Set the lower bound on the diagonal covariance values.
%     The bound can be either a scalar or a vector with one
%     entry per dimension. Using null bounds is possible, but
%     may yield degenerate solutions, including NaNs.
%
%   Example::
%     VL_GMM(X, 10, 'verbose', 'MaxNumIterations', 20) learns a
%     mixture of ten Gaussians using at most twenty iterations of the
%     algorithm.
%
%   See also: <a href="matlab:vl_help('gmm')">GMMs</a>, VL_KMEANS(), VL_HELP().

% Authors: David Novotny and Andrea Vedaldi

% Copyright (C) 2013 David Novotny and Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
