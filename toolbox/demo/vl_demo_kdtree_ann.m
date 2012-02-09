function vl_demo_kdtree_ann
% VL_DEMO_KDTREE
%   Demonstrates the use of a kd-tree for approximate nearest neighbor
%   (ANN) queries.

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

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
