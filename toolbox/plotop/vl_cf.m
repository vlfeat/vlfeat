function h = vl_cf(h0)
% VL_CF Creates a copy of a figure
%   VL_CF() creates a copy of the current figure and returns VL_CF(H0)
%   creates a copy of the figure(s) whose handle is H0.  H =
%   VL_CF(...) returns the handles of the copies.
%
%   See also: VL_HELP().

% Authors: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if nargin < 1
  h0 = gcf ;
end

h=zeros(size(h0)) ;
for i=1:numel(h0)
  h(i) = copyobj(h0(i),get(h0(i),'Parent')) ;
end

if nargout < 1
  clear h ;
end
