function h=plotgrid(x,y,varargin)
% PLOTGRID  Plot a 2-D grid
%  PLOTGRID(X,Y) plots a grid with vertices (X,Y). X and Y are MxN
%  matrices, with one entry per vertex.
%
%  H=PLOTGRID(...) returns the handle to the grid object.

washold = ishold ;

[M,N] = size(x) ;

hold on ;

xh = [x' ; nan*ones(1,M) ] ;
yh = [y' ; nan*ones(1,M) ] ;

xh = xh(:) ;
yh = yh(:) ;

xv = [x ; nan*ones(1,N) ] ;
yv = [y ; nan*ones(1,N) ] ;

xv = xv(:) ;
yv = yv(:) ;

h = line([xh' xv'], [yh' yv'],varargin{:}) ;

if ~washold
  hold off ;
end
