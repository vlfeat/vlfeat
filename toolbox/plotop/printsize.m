function printsize(r)
% PRINTSIZE  Set the print size of a figure
%   PRINTSIZE(R) set the print size of a figure so that the width is
%   the R fraction of the current paper size width.
%
%   This command is useful to resize figures before printing them so
%   that elements are scaled to match the final figure size in
%   print.
%
%  See also VLFEAT_HELP().

% AUTORIGHTS

set(gcf,'paperunits','normalized') ;
pos = get(gcf,'paperposition') ;
s = r/pos(3) ;
set(gcf,'paperposition',s*pos) ;
