% VL_VLAD   Compute the VLAD encoding
%   ENC = VL_FISHER(X, MEANS, ASSIGNMENTS) computes the VLAD
%   encoding of the vectors X relative to cluster centers MEANS and
%   vector-to-cluserr assignments ASSIGNMENTS.
%
%   X has one column per data vector (e.g. a SIFT descriptor), and
%   MEANS has one column per cluster. X and MEANS have the same number
%   of rows and data class, which can be either SINGLE or DOUBLE.
%
%   ASSIGNMENTS has as many rows as clusters and as many columns as
%   X. Its columns are non-negative and shoudl sum to one,
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
%   See: http://www.vlfeat.org/doc/api/vlad.html.

% Authors: , David Novotny and Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
