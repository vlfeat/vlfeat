function h = vl_demo_kdtree_plot(kdtree, nodeIndex, xlim, ylim)

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
h = line(xs,ys) ; %'b-') ;

if lowerChild > 1
  h = [h vl_demo_kdtree_plot(kdtree, lowerChild, xlimLower, ylimLower)] ;
end

if upperChild > 1
  h = [h vl_demo_kdtree_plot(kdtree, upperChild, xlimUpper, ylimUpper)] ;
end
