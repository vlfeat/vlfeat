% VL_VLAD   VLAD feature encoding
%   ENC = VL_VLAD(X, MEANS, ASSIGNMENTS) computes the VLAD
%   encoding of the vectors X relative to cluster centers MEANS and
%   vector-to-cluster soft assignments ASSIGNMENTS.
%
%   X has one column per data vector (e.g. a SIFT descriptor), and
%   MEANS has one column per component. Usually one has one component
%   per KMeans cluster and MEANS are the KMeans centers. X and MEANS
%   have the same number of rows and the data class, which can be
%   either SINGLE or DOUBLE.
%
%   ASSIGNMENTS has as many rows as clusters and as many columns as
%   X. Its columns are non-negative and should sum to one,
%   representing the soft assignment of the corresponding vector in X
%   to each of the clusters. It is of the same class as X.
%
%   ENC is a vector of the same class of X of size equal to the
%   product of the data dimension and the number of clusters.
%
%   By default, ENC is L2 normalized. VL_VLAD() accepts the following
%   options:
%
%   Unnormalized::
%     If specified, no overall normalization is applied to ENC.
%
%   NormalizeComponents::
%     If specified, the part of the encoding corresponding to each
%     cluster is individually normalized.
%
%   NormalizeMass::
%     If specified, each component is re-normalized by the mass
%     of data vectors assigned to it. If NormalizedComponents is
%     also selected, this has no effect.
%
%   SquareRoot::
%     If specified, the signed square root function is applied to
%     ENC before normalization.
%
%   Verbose::
%     Increase the verbosity level (may be specified multiple times).
%
%   See: <a href="matlab:vl_help('vlad')">VLAD</a>, VL_HELP().

% Authors: David Novotny and Andrea Vedaldi

% Copyright (C) 2013 David Novotny and Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
