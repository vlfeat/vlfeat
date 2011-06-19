function h=vl_plotgrid(x,y,varargin)
% VL_PLOTGRID  Plot a 2-D grid
%  VL_PLOTGRID(X,Y) plots a grid with vertices (X,Y). X and Y are MxN
%  matrices, with one entry per vertex.
%
%  H=VL_PLOTGRID(...) returns the handle to the grid object.
%
%  See also: VL_HELP().

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

lineprop = {} ;
if length(varargin) > 0
  lineprop = vl_linespec2prop(varargin{1}) ;
  lineprop = {lineprop{:}, varargin{2:end}} ;
end

h = line([xh' xv'], [yh' yv'],lineprop{:}) ;

if ~washold
  hold off ;
end
