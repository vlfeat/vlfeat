function [P1,P2] = vl_clicksegment
% VL_CLICKSEGMENT  Select a segment by clicking
%  [P1,P2] = VL_CLICKSEGMENT() lets the user select a segment in the
%  current figure and returns the starting and ending point P1 and P2.
%
%  The user can abort the operation by pressing any key. In this case
%  the function returns the empty matrix.
%
%  See also: VL_CLICK(), VL_CLICKPOINT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% Click first segment
P1=vl_click ;
if isempty(P1)
  P2=P1 ;
  return ;
end

was_hold = ishold(gca) ;
hold on ;
h=plot(P1(1),P1(2),'rx') ;

% Click second segment
P2=vl_click ;
if isempty(P2)
  P1=P2 ;
end
delete(h) ;
if ~was_hold, hold off ; end
