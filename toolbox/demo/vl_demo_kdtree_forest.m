function vl_demo_kdtree_forest
% VL_DEMO_KDTREE
%   Demonstrates the construction of a forest of randomized
%   kd-trees.

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

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
    xlim(xl) ;
    ylim(yl) ;
    axis square ;
    h = vl_demo_kdtree_plot(kdforest.trees(t), 1, xl, yl) ;
    title(sprintf('Tree number %d', t)) ;
    set(gca,'xtick',[],'ytick',[]) ;
    vl_demo_print(t,sprintf('kdtree_forest_tree_%d', t)) ;
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
  set(gca,'xtick',[],'ytick',[]) ;
  vl_demo_print(5,'kdtree_forest_query') ;
end
