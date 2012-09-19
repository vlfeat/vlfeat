function style = vl_plotstyle(index)
% VL_PLOTSTYLES  Get a plot style
%   STYLES = VL_PLOTSTYLE(INDEX) returns a cell array of PLOT() options
%   for the color/line style of index INDEX. The available styles are
%   defined the ColorOrder and LineStyleOrder properties of the
%   current axes and cycled first by color and then by line style.
%
%   If there is no current axes can be found, then VL_PLOTSTYLE() uses
%   the DefaultAxesColorOrder and DefaultLineStyleOrder root
%   properties.
%
%   If INDEXES has more than one element, then VL_PLOTSTYLE(INDEXES)
%   returns a cell array of styles of the same size of INDEXES.
%
%   Example::
%     The code fragment
%
%        set(gca,'LineStyleOrder', {'-','-.'}, 'NextPlot', 'replacechildren') ;
%        plot([1 2], [1:20 ; 1:20]') ;
%
%     and
%
%       set(gca,'NextPlot', 'replacechildren') ;
%       set(gca,'LineStyleOrder', {'-','-.'}) ;
%       for i = 1:20
%         style = vl_plotstyle(i) ;
%         plot([1 2], [i i], style{:}) ;
%         hold on ;
%       end
%
%     produce visually similar results.
%
%   See also: VL_HELP().

% Author: Andrea Vedaldi

colors = [] ;
lines = {} ;

% try to get from current axes, if any
fig = get(0, 'CurrentFigure') ;
if ~isempty(fig)
  ax = get(fig, 'CurrentAxes') ;
  if ~isempty(ax)
    colors = get(gca, 'ColorOrder') ;
    lines = get(gca, 'LineStyleOrder') ;
  end
end

% if not, pick matlab defaults
if isempty(colors)
  colors = get(0,'DefaultAxesColorOrder') ;
  lines = get(0,'DefaultAxesLineStyleOrder') ;
end

% make sure lines is a cell and not a char array
if ~iscell(lines)
  lines_ = lines ;
  lines = {} ;
  for i = 1:size(lines_,1)
    lines{i} = deblank(lines_(i,:)) ;
  end
end

% if more than one style specified
style = cell(size(index)) ;
for i = 1:numel(index)
  ci = mod(index(i) - 1, size(colors,1)) + 1 ;
  li = mod(floor((index(i) - 1) / size(colors,1)), numel(lines)) + 1 ;
  style{i} = {'Color', colors(ci,:), 'LineStyle', lines{li}} ;
end

% remove one layer of cell array if single style requested
if numel(index) == 1
  style = [style{:}] ;
end
