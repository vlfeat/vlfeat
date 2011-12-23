% VL_BINSUM  Binned summation
%   ACCUMULATOR = VL_BINSUM(ACCUMULATOR,VALUES,INDEXES) adds the
%   elements of the array VALUES to the elements of the array
%   ACCUMULATOR indexed by INDEXES. VALUES and INDEXES must have the
%   same dimensions, and the elements of INDEXES must be valid indexes
%   for the array ACCUMULATOR (except for null indexes, which are
%   silently skipped). An application is the calculation of a
%   histogram ACCUMULATOR, where INDEXES are the bin occurences and
%   VALUES are the occurence weights.
%
%   VL_BINSUM(...,DIM) operates only along the specified dimension
%   DIM. In this case, ACCUMULATOR, VALUES and INDEXES are array of
%   the same dimensions, except for the dimension DIM of ACCUMULATOR,
%   which may differ, and INDEXES is an array of subscripts of the
%   DIM-th dimension of ACCUMULATOR. A typical application is the
%   calculation of multiple histograms, where each histogram is a
%   1-dimensional slice of the array ACCUMULATOR along the dimension
%   DIM.
%
%   VALUES can also be a scalar. In this case, the value of VALUES is
%   summed to all the specified bins.
%
%   ACCUMULATOR and VALUES must have the same storage
%   class. ACCUMULATOR, VALUES, and INDEXES can be either DOUBLE,
%   SINGLE, UINT64, INT64, UINT32, INT32, UINT16, INT16, UINT8,
%   INT8. Integer math uses modulo-2 arithmetic (most MATLAB functions
%   saturate instead).
%
%   Example::
%     The following relations illustrate VL_BINSUM() behavior:
%       VL_BINSUM([0 0],  1, 2) = [0 1]
%       VL_BINSUM([1 7], -1, 1) = [0 7]
%       VL_BINSUM(EYE(3), [1 1 1], [1 2 3], 1) = 2*EYE(3)
%
%   See also: VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
