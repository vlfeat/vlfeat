function vl_demo_kdtree_ann
% VL_DEMO_KDTREE

randn('state',0) ;
rand('state',0) ;

X = single(rand(2, 100)) ;
kdtree = vl_kdtreebuild (X) ;

Q = single(rand(2,1)) ;

maxr = [1 10 20 30] ;
for t = [1 2 3 4]
  figure(t) ; clf ;
  vl_plotframe(X, 'ro') ;
  hold on ;
  xl = [.2, .8] ;
  yl = [.1, .6] ;
  xlim(xl) ;
  ylim(yl) ;
  axis equal ;
  %  vl_demo_kdtree_plot(kdtree, 1, xl, yl) ;

  [i, d] = vl_kdtreequery (kdtree, X, Q, ...
                           'numneighbors', 10, ...
                           'maxcomparisons', maxr(t), ...
                           'verbose') ;

  plotframe(Q,'b*','markersize',10) ;
  for k=1:length(i)
    if i(k) == 0, continue ; end
    plotframe([Q ; sqrt(d(k))],'b-','linewidth',1) ;
    plotframe(X(:, i(k)), 'bx','markersize',15) ;
  end
  title(sprintf('10 ANNs with at most %d comparisions', maxr(t))) ;
end
