% VL_FISHER    Fisher vector feature encoding
%   ENC = VL_FISHER(X, MEANS, COVARIANCES, PRIORS) computes the
%   Fisher vector encoding of the vectors X relative to the Gaussian
%   mixture model with means MEANS, covariances COVARIANCES, and pror
%   mode probabilities PRIORS.
%
%   X has one column per data vector (e.g. a SIFT descriptor), and
%   MEANS and COVARIANCES one column per GMM component (covariance
%   matrices are assumed diagonal). PRIORS has size equal to the
%   number of GMM components. All data must be of the smae class,
%   either SINGLE or DOUBLE.
%
%   ENC is a vector of the same class of X of size equal to the
%   product of the data dimension and the number of components.
%
%   By default, the standard Fisher vector is computed. VL_FISHER()
%   accepts the following options:
%
%   Normalized::
%     If specified, L2 normalize the Fisher vector.
%
%   SquareRoot::
%     If specified, the signed square root function is applied to
%     ENC before normalization.
%
%   Verbose::
%     Increase the verbosity level (may be specified multiple times).
%
%   See: <a href="matlab:vl_help('fisher')">Fisher vectors</a>, VL_HELP().

% Authors: David Novotny, Andrea Vedaldi

% Copyright (C) 2013 David Novotny and Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
