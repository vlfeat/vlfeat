function vl_demo_kdtree
% VL_DEMO_KDTREE
%   Demonstrates the construction of a kd-tree for uniformly
%   distributed 2-D data and data distributed on a circle. Both the
%   median and mean threshold selection methods are illustrated.

  randn('state',0) ;
  rand('state',0) ;

  % uniform 2D points
  Q = single(rand(2,1)) ;
  X = single(rand(2, 100)) ;

  % kd-tree with median and mean thresholds
  figure(1) ; clf ; do('thresholdmethod', 'mean') ;
  figure(2) ; clf ; do('thresholdmethod', 'median') ;

  % 2D points on a circle
  thr = 2*pi*rand(1,100) ;
  X = single(.4*[cos(thr); sin(thr)] + .5) ;
  Q = single(Q) ;

  % kd-tree with median and mean thresholds
  figure(3) ; clf ; do('thresholdmethod', 'mean') ;
  figure(4) ; clf ; do('thresholdmethod', 'median') ;

  vl_demo_print(1, 'kdtree_uniform_mean') ;
  vl_demo_print(2, 'kdtree_uniform_median') ;
  vl_demo_print(3, 'kdtree_circle_mean') ;
  vl_demo_print(4, 'kdtree_circle_median') ;

  function do(varargin)
    % build the forest
    kdforest = vl_kdtreebuild (X, 'verbose', varargin{:}) ;

    % query the kd-tree
    [i, d] = vl_kdtreequery (kdforest, X, Q, 'numneighbors', 10, 'verbose') ;
    vl_plotframe(X, 'ro') ;
    hold on ;
    xl = [0, +1] ;
    yl = [0, +1] ;
    axis equal ;
    xlim(xl) ;
    ylim(yl) ;
    vl_demo_kdtree_plot(kdforest.trees(1), 1, xl, yl) ;

    % plot query point and returns
    vl_plotframe(Q,'b*') ;
    for k=1:length(i)
      vl_plotframe([Q ; sqrt(d(k))],'b:','linewidth',1) ;
      vl_plotframe(X(:, i(k)), 'bx') ;
    end
    set(gca,'xtick',[],'ytick',[]) ;
  end

end
