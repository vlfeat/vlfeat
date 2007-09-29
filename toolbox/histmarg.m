function H = histmarg(H, dims)
% HISTMARG  Marginal of histogram
%  H = HISTMARG(H, DIMS) marginalizes the historgram HISTMARG w.r.t
%  the dimensions DIM. This is done by summing out all dimensions
%  not listed in DIM and deleting them.
%
%  REMARK. If DIMS lists only one dimension, the returned H is a
%  column vector. Notice that this way of deleting dimensions is not
%  always consistent with the SQUEEZE function.

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

sz = size(H) ;

for d=setdiff( 1:length(sz), dims(:) )
  H = sum(H, d) ;
end

% Squeeze out marginalized dimensions
sz = sz(dims(:)) ;
sz = [sz ones(1,2-length(dims(:)))] ;
H = reshape(H, sz) ;
