function vl_demo_kdtree_ann
% VL_DEMO_KDTREE
%   Demonstrates the use of a kd-tree for approximate nearest neighbor
%   (ANN) queries.

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

randn('state',0) ;
rand('state',0) ;

% Generate some 2D data and a query point
X = rand(2, 100) ;
Q = rand(2,1) ;

% Buld a kd-tree
kdtree = vl_kdtreebuild(X) ;

% Query with increasing accuracy
maxNumComparisonRange = [1 10 20 30] ;
for t = [1 2 3 4]
  figure(t) ; clf ;
  vl_plotframe(X, 'ro') ;
  hold on ;
  xl = [.2, .8] ;
  yl = [.1, .7] ;
  xlim(xl) ;
  ylim(yl) ;

  %  vl_demo_kdtree_plot(kdtree, 1, xl, yl) ;

  [i, d] = vl_kdtreequery (kdtree, X, Q, ...
                           'NumNeighbors', 10, ...
                           'MaxComparisons', maxNumComparisonRange(t), ...
                           'Verbose') ;

  vl_plotframe(Q,'b*','markersize',10) ;
  for k=1:length(i)
    if i(k) == 0, continue ; end
    vl_plotframe([Q ; sqrt(d(k))],'b-','linewidth',1) ;
    vl_plotframe(X(:, i(k)), 'bx','markersize',15) ;
  end
  title(sprintf('10 ANNs with at most %d comparisions', maxNumComparisonRange(t))) ;

  axis square ;
  set(gca,'xtick',[],'ytick',[]) ;
  vl_demo_print(t, sprintf('kdtree_ann_%d', t)) ;
end
