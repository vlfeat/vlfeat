function vl_demo_print(varargin)
% VL_DEMO_PRINT
%   VL_DEMO_PRINT(NAME) prints the current figure to the documentation
%   directory with the specified filename, assuming that the global
%   variable VL_DEMO_PRINT is defined and non-empty when MATLAB is
%   started (our using SETENV() from MATLAB). Otherwise the function
%   flushes the displays and returns.
%
%   VL_DEMO_PRINT(NAME, R) specifies a magnification factor R, setting
%   the figure width relatively to the page width. If not specified, R
%   is assumed to be 1/2.
%
%   Remarks:: The figure paper type is set to letter, that has size 8.5 x
%     11 inches. When converted for web viewing, images are rasterized
%     at either 75 or 95 DPI, The documentation system converts images
%     to bitmap with a resolution of 75 DPI, which makes a letter size
%     page 637 or 808 pixels large, repsectively.
%
%     In MATLAB font sizes are usually expressed in points, where a
%     point is a 1/72 inch. Thus a 12pt font sampled at 75 DPI is
%     about 12.5 pixels high.

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if isempty(getenv('VL_DEMO_PRINT'))
  drawnow ;
  return ;
end

if isa(varargin{1}, 'double')
  fig = varargin{1} ;
  varargin(1) = [] ;
else
  fig = gcf ;
end

name = varargin{1} ;

if length(varargin) < 2
  figurePaperSize = 0.5 ;
else
  figurePaperSize = varargin{2} ;
end

vl_printsize(fig, figurePaperSize) ;

figDir = fullfile(vl_root,'doc','demo') ;
if ~ exist(figDir, 'dir')
  mkdir(figDir) ;
end

if 0
  filePath = fullfile(figDir, [name '.eps']) ;
  print(fig, '-depsc2', filePath) ;
else
  filePath = fullfile(figDir, [name '.jpg']) ;
  print(fig, '-djpeg95', filePath, '-r95') ;
end
fprintf('%s: wrote file ''%s''\n', mfilename,  filePath) ;
