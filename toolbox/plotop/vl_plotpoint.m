function h=vl_plotpoint(V,varargin)
% VL_PLOTPOINT  Plot 2 or 3 dimensional points
%   VL_PLOTPOINT(V) plots the 2 or 3 dimensional points V. V is a 2xK or
%   3xK array, with one point per column.
%
%   H=VL_PLOTPOINT(...) returns the handle H of the plot.
%
%   VL_PLOTPOINT() is a simple wrapper around the PLOT() and PLOT3()
%   functions. By default, VL_PLOTPOINT(V) plots the points with line
%   style '.'.  VL_PLOTPOINT(V,...) does not use the default line style;
%   rather it passess any extra argument to the underlying plot
%   function.
%
%   See also: PLOT(), PLOT3(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if length(varargin) == 0
  varargin = {'.'};
end

switch size(V,1)
  case 2
    h=plot(V(1,:),V(2,:),varargin{:}) ;
  case 3
    h=plot3(V(1,:),V(2,:),V(3,:),varargin{:}) ;
  otherwise
    error(['V must be either 2xK or 3xK.']) ;
end
