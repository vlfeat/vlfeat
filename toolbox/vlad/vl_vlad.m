% VL_VLAD retrieve VLAD encoding of a given set of data
%   [ENC] = VL_FISHER(X, MEANS, ASSIGN) 
%   retrieves the vlad encoding of a set of vectors. Data vectors
%   are stored in columns of matrix X, the means of individual
%   clusters are also located in columns of the MEANS matrix.
%   Assignments are soft or hard assignments organized in the matrix,
%   such that:
%   [ p(x1|c1), p(x2|c1), ... p(xN|c1);
%     p(x1|c2), p(x2|c2), ... p(xN|c2);
%     ...
%     p(x1|cK), p(x2|cK), ... p(xN|cK) ] = POSTERIORS
%
%   To express a hard assignment, insert a 1(assigned) or zero to
%   the corresponding place in the ASSIGN matrix
%
%   Verbose::
%     Increase the verbosity level (may be specified multiple times).
%
% Authors: Andrea Vedaldi, David Novotny
%
% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
