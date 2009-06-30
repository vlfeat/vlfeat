% VL_BINSEARCH  Binary search
%   IDX = VL_BINSEARCH(B, X) maps the elements of X to the bins
%   defined by B. B is a non-decreasing vector of M real numbers
%   defining bins [-inf, B(1)), [B(1), B(2)), ... [B(M) +inf] (note
%   that the last bin contains +inf). The bins are numbered from
%   0 to M. X is a matrix of real numbers and IDX is a matrix of the
%   same size as X, with the number of the bin to which each element
%   of X belongs.
%
%   See also:: VL_BINSUM().
