function vl_figaspect(a)
% VL_FIGASPECT  Set figure aspect ratio
%   VL_FIGASPECT(A) sets the current figure aspect ratio to A. It
%   leaves the length of the shortest side unaltered. Both the
%   screen and paper positions are affected.
%
%   See also: VL_PRINTSIZE(), VL_HELP().

% Authors: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

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
