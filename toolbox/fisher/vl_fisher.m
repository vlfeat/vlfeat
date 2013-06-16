% VL_FISHER retrieve fisher encoding of a given set of data
%   [ENC] = VL_FISHER(X, MEANS, SIGMAS, WEIGHTS) 
%   retrieves the fisher encoding of a set of vectors. Data vectors
%   are stored in columns of matrix X, the means of individual
%   clusters are also located in columns of the MEANS matrix.
%   The columns of SIGMAS contain the diagonals of variance matrices.
%   weights is a column vector consisting of the weights of individiual
%   clusters.
%
%   Verbose::
%     Increase the verbosity level (may be specified multiple times).

% Authors: David Novotny, Andrea Vedaldi

% Copyright (C) 2013 David Novotny and Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
