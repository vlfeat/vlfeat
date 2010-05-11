function vl_figaspect(a)
% VL_FIGASPECT  Set figure aspect ratio
%   VL_FIGASPECT(A) sets the current figure aspect ratio to A. It
%   leaves the length of the shortest side unaltered. Both the
%   screen and paper positions are affected.
%
%   See also: VL_HELP(), VL_PRINTSIZE().

% Author: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

pos = get(gcf, 'Position') ;

if a >= 1
  pos(3) = a * pos(4) ;
else
  pos(4) = pos(3) / a ;
end

set(gcf,'Position',pos) ;

pos = get(gcf, 'PaperPosition') ;

if a >= 1
  pos(3) = a * pos(4) ;
else
  pos(4) = pos(3) / a ;
end

set(gcf,'PaperPosition', pos) ;
