function [Y, sel] = vl_colsubset(X,n,varargin)
% VL_COLSUBSET Select a given number of columns
%   Y = VL_COLSUBSET(X, N) returns a random subset Y of N columns of
%   X. The selection is order-preserving and without replacement. If N
%   is larger or equal to the number of columns of X (e.g. N = Inf),
%   then the function returns all the columns (i.e., Y = X).
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
%     Returns the fist N columns.
%
%   Ending::
%     Returns the last N columns.
%
%   Random:: [default]
%     Returns N columns selected at random (using RANDPERM()).
%
%   Uniform::
%     Returns N uniformly spaced columns.
%
%   Largest::
%     Returns the N largest columns (using SORTROWS()).
%
%   Smallest::
%     Returns the N smallest columns (using SORTROWS()).
%
%  See also: VL_HELP().

% Authors: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if nargin < 2, n = 1 ; end

mode = 'random' ;
i = 1 ;
while i <= length(varargin)
  switch lower(varargin{i})
    case {'beginning', ...
          'ending', ...
          'random', ...
          'uniform', ...
          'largest', ...
          'smallest'}
      mode = lower(varargin{1}) ;
      i = i + 1 ;
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
  case 'largest'
    [drop, perm] = sortrows(X') ;
    sel = sort(perm(end-n+1:end)) ;
  case 'smallest'
    [drop, perm] = sortrows(X') ;
    sel = sort(perm(1:n)) ;
end

Y = X(:, sel) ;
