function vl_demo_kdtree_forest
% VL_DEMO_KDTREE

  randn('state',0) ;
  rand('state',0) ;

  Q = single(rand(2,1)) ;

  X = single(rand(2, 100)) ;

  kdforest = vl_kdtreebuild (X,  ...
                             'verbose', ...
                             'numtrees', 4) ;
  for t=1:4
    figure(t) ; clf ;
    vl_plotframe(X, 'ro') ;
    hold on ;
    xl = [0, +1] ;
    yl = [0, +1] ;
    axis equal ;
    xlim(xl) ;
    ylim(yl) ;
    h = vl_demo_kdtree_plot(kdforest.trees(t), 1, xl, yl) ;
  end

  figure(5) ; clf ;
  vl_plotframe(X, 'ro') ; hold on ;
  [i, d] = vl_kdtreequery (kdforest, X, Q, 'numneighbors', 10, 'verbose') ;

  plotframe(Q,'b*') ;
  for k=1:length(i)
    plotframe([Q ; sqrt(d(k))],'b:','linewidth',1) ;
    plotframe(X(:, i(k)), 'bx') ;
  end
end
