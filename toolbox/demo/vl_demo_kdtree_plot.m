function h = vl_demo_kdtree_plot(kdtree, nodeIndex, xlim, ylim)
% VL_DEMO_KDTREE_PLOT

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

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
