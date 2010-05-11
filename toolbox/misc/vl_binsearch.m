% VL_BINSEARCH  Maps data to bins
%   IDX = VL_BINSEARCH(B, X) maps the elements of X to the bins
%   defined by B. B is a non-decreasing vector of M real numbers
%   defining bins [-inf, B(1)), [B(1), B(2)), ... [B(M) +inf] (note
%   that the last bin contains +inf). The bins are numbered from 0 to
%   M. X is a real (plain) array and IDX is an array with the same
%   dimensions of X, specifying the bin correpsonding to each element
%   of X.
%
%   If B(1) = -inf the bin number 0 is removed and the bin number 1 is
%   [-inf B(2)). If B(M) = +inf, the last bin M is the singleton +inf.
%
%   See also: VL_BINSUM().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

