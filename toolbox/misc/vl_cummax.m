% VL_CUMMAX  Cumulative maximum
%   If X is a vector, VL_CUMMAX(X) is the vector with the cumulative
%   maximum of X. If X is a matrix, VL_CUMMAX(X) is the matrix of the
%   cumulative maximum of the columns of X. If X is a multi-dimensional
%   array, VL_CUMMAX(X) operates along the first non-singleton
%   dimension.
%
%   The function is analogous to CUMSUM() and CUMPROD(), but it
%   computes the cumulative maximum rather than the cumulative sum or
%   product.
%
%   The array X can be of any numeric class.
%
%   Example::
%     The cumulative maximum VL_CUMMAX([1 2 3 2 1]) is the vector
%     [1 2 3 3 3].
%
%   See also: CUMSUM(), CUMPROD(), VL_HELP().

% Author: Andrea Vedaldi

% Copyright
