function vl_demo_kdtree_self
% VL_DEMO_KDTREE_SELF
%   Demonstrates using a kd-tree to find the pairwise nearest
%   neighbors in a set of points.

% AUTORIGHTS

randn('state',0) ;
rand('state',0) ;

% a set of 2D pints
X = single(rand(2, 100)) ;

% build the kdtree
kdtree = vl_kdtreebuild(X) ;

% query the kdtree with X itself
[i, d] = vl_kdtreequery(kdtree, X, X, 'numneighbors', 2, 'verbose') ;

% plot
figure(1) ; clf ;
vl_plotframe(X, 'ro') ;
hold on ;
xl = [0, +1] ;
yl = [0, +1] ;
axis equal ;
xlim(xl) ;
ylim(yl) ;
vl_demo_kdtree_plot(kdtree.trees(1), 1, xl, yl) ;
line([X(1,:) ; X(1,i(2,:))], ...
     [X(2,:) ; X(2,i(2,:))], 'linewidth', 2) ;
vl_demo_print('kdtree_self') ;
