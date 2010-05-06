function vl_demo_kdtree_forest
% VL_DEMO_KDTREE
%   Demonstrates the construction of a forest of randomized
%   kd-trees.

% AUTORIGHTS

  % numer of trees in the forest
  numTrees = 4 ;

  randn('state',0) ;
  rand('state',0) ;

  % 2D data X and a query point
  Q = single(rand(2,1)) ;
  X = single(rand(2, 100)) ;

  % build  the forest
  kdforest = vl_kdtreebuild (X,  ...
                             'verbose', ...
                             'numtrees', numTrees) ;

  % plot each tree of the forest
  for t=1:numTrees
    figure(t) ; clf ;
    vl_plotframe(X, 'ro') ;
    hold on ;
    xl = [0, +1] ;
    yl = [0, +1] ;
    axis equal ;
    xlim(xl) ;
    ylim(yl) ;
    h = vl_demo_kdtree_plot(kdforest.trees(t), 1, xl, yl) ;
    vl_demo_print(sprintf('kdtree_forest_tree_%d', t)) ;
  end

  % query
  [i, d] = vl_kdtreequery (kdforest, X, Q, 'numneighbors', 10, 'verbose') ;

  % show query and return
  figure(5) ; clf ;
  vl_plotframe(X, 'ro') ; hold on ;
  vl_plotframe(Q,'b*') ;
  for k=1:length(i)
    vl_plotframe([Q ; sqrt(d(k))],'b:','linewidth',1) ;
    vl_plotframe(X(:, i(k)), 'bx') ;
  end
  xlim([0 1]) ;
  ylim([0 1]) ;
  axis square ;
  vl_demo_print('kdtree_forest_query') ;
end
