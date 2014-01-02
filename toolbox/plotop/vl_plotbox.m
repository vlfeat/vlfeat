function [h, t] = vl_plotbox(boxes, varargin)
% VL_PLOTBOX  Plot boxes
%   VL_PLOTBOX(BOX) plots a box specified by the 4-dimensional column
%   vector BOXES = [XMIN YMIN XMAX YMAX]'. If BOXES is a 4 x N matrix,
%   a box for each of the N columns is plotted.
%
%   H = VL_PLOTBOX(BOXES) returns a handle to the line drawing
%   representing the boxes. For multiple boxes, H is a row vector with
%   one handle per box.
%
%   VL_PLOTBOX(BOXES, 'LABEL', LABEL) annotates the box with the
%   string LABEL. If BOXES contains multiple boxes, then LABEL can be
%   a cell array with one entry for each box. H is then a 2 x N array
%   with handles to boxes and corresponding labels.
%
%   VL_PLOTBOX(BOXES, ...) passes any extra argument to the underlying
%   plotting function. The first optional argument can be a line
%   specification string such as the one used by MATLAB's PLOT()
%   function.
%
%   See also:: VL_PLOTFRAME().

% Author:: Andrea Vedaldi

% Copyright (C) 2008-13 Andrea Vedaldi
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.label = {} ;

% if the first optional argument is a linespec expand it
if length(varargin) > 0
  lineprop = vl_linespec2prop(varargin{1}) ;
  varargin = {lineprop{:}, varargin{2:end}} ;
end

% parse optional arguments
[opts, varargin] = vl_argparse(opts, varargin) ;
if ischar(opts.label)
  opts.label = {opts.label} ;
end

if size(boxes,2) == 0
  h = [] ;
  return ;
end

if size(boxes,1) ~= 4
  error('BOXES must be a 4 x N matrix') ;
end

Lx = [1 0 0 0 ;
      0 0 1 0 ;
      0 0 1 0 ;
      1 0 0 0 ;
      1 0 0 0 ] ;

Ly = [0 1 0 0 ;
      0 1 0 0 ;
      0 0 0 1 ;
      0 0 0 1 ;
      0 1 0 0 ] ;

fig = newplot ;
h = line(Lx * boxes, Ly * boxes, varargin{:}) ;
h = h' ;

if ~isempty(opts.label)
  ish = ishold ;
  hold on ;
  t = zeros(1,length(opts.label)) ;
  for r = 1:size(boxes,2)
    cl = get(h(1,r), 'Color') ;
    q = mod(r - 1, length(opts.label)) + 1 ;
    h(2,r) = text(mean(boxes([1 3],r)), boxes(2,r), opts.label{q}, ...
                  'Background', cl, ...
                  'VerticalAlignment', 'bottom', ...
                  'HorizontalAlignment', 'center') ;
  end
  if ~ish, hold off ; end
end

if nargout == 0
  clear h ;
end
