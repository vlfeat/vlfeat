function vl_demo_print(varargin)
% VL_DEMO_PRINT
%   VL_DEMO_PRINT(NAME) print the current figure to the
%   documentation directory with the specified filename.
%
%   VL_DEMO_PRINT(NAME, R) specifies a magnification factor R, which
%   express the figure width relative to the page width. If not
%   specified, R is assumed to be 1/2.
%
%   Remarks:: The figure paper type is set to letter, which is 8.5 x
%     11 inches. When converted for web viewing, images are rasterized
%     at either 75 or 95 DPI, The documentation system converts images
%     to bitmap with a resolution of 75 DPI, which makes a letter size
%     page 637 or 808 pixels large, repsectively.
%
%     In MATLAB font sizes are usually expressed in points, where a
%     point is a 1/72 inch. Thus a 12pt font sampled at 75 DPI is
%     about 12.5 pixels high.

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

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

filePath = fullfile(figDir, [name '.eps']) ;
print(fig, '-depsc2', filePath) ;
fprintf('%s: wrote file ''%s''\n', mfilename,  filePath) ;
