function vl_demo_kdtree
% VL_DEMO_KDTREE

  randn('state',0) ;
  rand('state',0) ;

  Q = single(rand(2,1)) ;

  X = single(rand(2, 100)) ;

  figure(1) ; clf ; do('thresholdmethod', 'mean') ;
  figure(2) ; clf ; do('thresholdmethod', 'median') ;

  thr = 2*pi*rand(1,100) ;
  X = single(.4*[cos(thr); sin(thr)] + .5) ;

  figure(3) ; clf ; do('thresholdmethod', 'mean') ;
  figure(4) ; clf ; do('thresholdmethod', 'median') ;

  function do(varargin)
    kdforest = vl_kdtreebuild (X, 'verbose', varargin{:}) ;
    vl_plotframe(X, 'ro') ;
    hold on ;
    xl = [0, +1] ;
    yl = [0, +1] ;
    axis equal ;
    xlim(xl) ;
    ylim(yl) ;
    vl_demo_kdtree_plot(kdforest.trees(1), 1, xl, yl) ;

    [i, d] = vl_kdtreequery (kdforest, X, Q, 'numneighbors', 10, 'verbose') ;

    plotframe(Q,'b*') ;
    for k=1:length(i)
      plotframe([Q ; sqrt(d(k))],'b:','linewidth',1) ;
      plotframe(X(:, i(k)), 'bx') ;
    end
  end

end
