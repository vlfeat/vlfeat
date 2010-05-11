function H = vl_histmarg(H, dims)
% VL_HISTMARG  Marginal of histogram
%   H = VL_HISTMARG(H, DIMS) marginalizes the historgram H w.r.t the
%   dimensions DIMS. This is done by summing out all dimensions not
%   listed in DIMS and deleting them.
%
%   Remark::
%     If DIMS lists only one dimension, the returned histogram H is a
%     column vector. Notice that this way of deleting dimensions is
%     not always consistent with the SQUEEZE function.
%
%   See also: VL_HELP().

% Author: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

sz = size(H) ;

for d=setdiff(1:length(sz), dims(:))
  H = sum(H, d) ;
end

% Squeeze out marginalized dimensions
sz = sz(dims(:)) ;
sz = [sz ones(1,2-length(dims(:)))] ;
H = reshape(H, sz) ;
