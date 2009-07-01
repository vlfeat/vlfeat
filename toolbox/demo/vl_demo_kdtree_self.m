function vl_demo_kdtree_self
% VL_DEMO_KDTREE_SELF

randn('state',0) ;
rand('state',0) ;

X = single(rand(2, 100)) ;
kdtree = vl_kdtreebuild (X) ;
[i, d] = vl_kdtreequery (kdtree, X, X, 'numneighbors', 2, 'verbose') ;

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


