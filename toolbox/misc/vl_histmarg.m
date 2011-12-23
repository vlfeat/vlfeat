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

% Authors: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

sz = size(H) ;

for d=setdiff(1:length(sz), dims(:))
  H = sum(H, d) ;
end

% Squeeze out marginalized dimensions
sz = sz(dims(:)) ;
sz = [sz ones(1,2-length(dims(:)))] ;
H = reshape(H, sz) ;
