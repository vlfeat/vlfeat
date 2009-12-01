function vl_printsize(r,a)
% VL_PRINTSIZE  Set the print size of a figure
%   VL_PRINTSIZE(R) set the print size of a figure so that the width is
%   the R fraction of the current paper size width.
%
%   VL_PRINTSIZE(R,A) sets also the aspect ratio to A (the aspect ratio
%   is the width divided by the height)
%
%   This command is useful to resize figures before printing them so
%   that elements are scaled to match the final figure size in print.
%
%   See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if nargin < 2
  a = NaN ;
end

papuni = get(gcf,'paperunits') ;
set(gcf,'paperunits','normalized') ;
pos = get(gcf,'paperposition') ;
pos(1:2) = 0 ;
s = r/pos(3) ;
pos(3:4) = pos(3:4) * s ;
if ~isnan(a)
  pos(4) = pos(3) / a ;
end
set(gcf,'paperposition',pos) ;
set(gcf,'paperunits',papuni) ;
