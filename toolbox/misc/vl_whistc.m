function h = vl_whistc(x, w, edges, dim)
% VL_WHISTC Weighted histogram
%   H = VL_WHISTC(X,W,EDGES) behaves exactly like HISTC(X,EDGES), but
%   weights the samples X by W. Samples that have NaN weight are
%   skipped.
%
%   See also: HITSC(), VL_HELP().

% Authors: Andrea Vedladi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

numEdges = numel(edges) ;

% map x to the bins defined by edges
binIndexes = vl_binsearch(edges, x) ;

% binsearch last bin is [edges(end) +inf] but for histc it is the
% signleton [edges(end)]
if numEdges > 0
  binIndexes(binIndexes == numEdges & x > edges(end)) = 0 ;
end

% NaNs must be also removed
binIndexes(isnan(x)) = 0 ;

% find operating dimension
dimensions = size(x) ;
if nargin < 4
  nonSingletonDims = find(dimensions > 1) ;
  if ~ isempty(nonSingletonDims)
    dim = nonSingletonDims(1) ;
  else
    dim = 2 ; % assume row vector
  end
end

% accumulate
dimensions(dim) = numEdges ;
h = zeros(dimensions) ;
h = vl_binsum(h, w, binIndexes, dim) ;
