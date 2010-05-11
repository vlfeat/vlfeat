function sel = vl_clickpoint(V,N,varargin)
% VL_CLICKPOINT  Select a point by clicking
%   SEL = VL_CLICKPOINT(V) let the user vl_click a point in the current
%   figure and returns the index v of the closest point (in Euclidean
%   norm) in the collection V. The 2xK matrix V has a a column for
%   each point.
%
%   The user can abort the operation by pressing any key. In this case
%   the function returns the empty matrix.
%
%   VL_CLICKPOINT(V,N) selects N points in a row. The user can stop the
%   selection at any time by pressing any key. In this case the
%   partial selection is returned. This can be used in combination
%   with N=inf to get an arbitrary number of points.
%
%   VL_CLICKPOINT() accepts the following options:
%
%   PlotMarker [0]::
%     Put a marker as points are selected. The markers are deleted on
%     exiting the function.
%
%   See also:: VL_CLICK(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

plot_marker = 0 ;
for k=1:2:length(varargin)
  switch lower(varargin{k}) 
    case 'plotmarker'
      plot_marker = varargin{k+1} ;
    otherwise
      error(['Uknown option ''', varargin{k}, '''.']) ;
  end
end

if nargin < 2
  N=1;
end

if size(V,1) ~= 2
  error('Array V should be 2xK') ;
end

% --------------------------------------------------------------------
%                                                               Do job
% --------------------------------------------------------------------

fig = gcf ;
is_hold = ishold(fig) ;
hold on ;

sel = [] ;
h = [] ;
for n=1:N 
  P=vl_click ;
  if ~isempty( P )
    d = (V(1,:)-P(1)).^2 + (V(2,:)-P(2)).^2;
    [drop,v]=min(d(:)) ;   
    if(plot_marker)
      h=[h plot(V(1,v),V(2,v),'go')] ;
    end
    sel = [sel v] ;
  else
    return ;
  end
end

if ~is_hold
  hold off ;  
end

if( plot_marker )
  pause(.1);
  delete(h) ;
end



