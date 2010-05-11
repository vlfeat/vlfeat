function h = vl_demo_kdtree_plot(kdtree, nodeIndex, xlim, ylim)
% VL_DEMO_KDTREE_PLOT

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

dim        = kdtree.nodes.splitDimension(nodeIndex) ;
thresh     = kdtree.nodes.splitThreshold(nodeIndex) ;
lowerChild  = kdtree.nodes.lowerChild(nodeIndex) ;
upperChild = kdtree.nodes.upperChild(nodeIndex) ;

if dim == 1
  xs = [thresh, thresh] ;
  ys = ylim ;
  ylimLower = ylim ;
  ylimUpper = ylim ;
  xlimLower = [xlim(1) thresh] ;
  xlimUpper = [thresh xlim(2)] ;
else
  xs = xlim ;
  ys = [thresh, thresh] ;
  xlimLower = xlim ;
  xlimUpper = xlim ;
  ylimLower = [ylim(1) thresh] ;
  ylimUpper = [thresh ylim(2)] ;
end
h = line(xs,ys) ;

if lowerChild > 1
  h = [h vl_demo_kdtree_plot(kdtree, lowerChild, xlimLower, ylimLower)] ;
end

if upperChild > 1
  h = [h vl_demo_kdtree_plot(kdtree, upperChild, xlimUpper, ylimUpper)] ;
end
