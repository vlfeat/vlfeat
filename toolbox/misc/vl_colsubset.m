function [Y, sel] = vl_colsubset(X,n,varargin)
% VL_COLSUBSET Select a given number of columns
%   Y = VL_COLSUBSET(X, N) returns a random subset Y of N columns of
%   X. The selection is order-preserving and without replacement. If N
%   is larger or equal to the number of columns of X (e.g. N=+Inf),
%   then the function returns Y = X.
%
%   If 0 < N < 1, then the function returns a fraction N of the
%   columns (rounded to the closest integer).
%
%   [Y, SEL] = VL_COLSUBSET(...) returns the indexes SEL of the
%   selected columns.
%
%   The function accepts the following options:
%
%   Beginning::
%     Causes the first N columns to be returned.
%
%   Ending::
%     Causes the last N columns to be returned.
%
%   Random::
%     Causes a random selection of columns to be returned (default).
%
%   Uniform::
%     Causes N equally spaced columns to be returned.
%
%  See also: VL_HELP().

% Authors: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if nargin < 2, n = 1 ; end

mode = 'random' ;
i = 1 ;
while i <= length(varargin)
  switch lower(varargin{i})
    case 'beginning'
      mode = 'beginning' ; i = i + 1 ;
    case 'ending'
      mode = 'ending' ; i = i + 1 ;
    case 'random' ;
      mode = 'random' ; i = i + 1 ;
    case 'uniform'
      mode = 'uniform' ; i = i +1 ;
    otherwise
      error('Unknown option ''%s''.', varargin{i}) ;
  end
end


m = size(X,2) ;

if n < 0, error('N must not be smaller than 0.') ; end
if n ~= round(n)
  if n > 1
    error('N must be a natural number, +inf, or a fraction in 0 and 1.') ;
  end
  n = round(m * n) ;
end

n = min(m,n) ;

switch mode
  case 'random'
    perm = randperm(m) ;
    sel  = sort(perm(1:n)) ;
  case 'beginning'
    perm = 1:m ;
    sel  = sort(perm(1:n)) ;
  case 'ending'
    perm = m:-1:1 ;
    sel  = sort(perm(1:n)) ;
  case 'uniform'
    if n < 1
      sel = [] ;
    else
      sel = round(linspace(1, m, min(m,n))) ;
    end
end

Y = X(:, sel) ;
