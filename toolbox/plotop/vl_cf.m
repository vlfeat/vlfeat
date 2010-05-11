function h = vl_cf(h0)
% VL_CF Creates a copy of a figure
%   VL_CF() creates a copy of the current figure and returns VL_CF(H0)
%   creates a copy of the figure(s) whose handle is H0.  H =
%   VL_CF(...) returns the handles of the copies.
%
%   See also: VL_HELP().

% Author: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

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
