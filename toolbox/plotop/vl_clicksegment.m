function [P1,P2] = vl_clicksegment
% VL_CLICKSEGMENT  Select a segment by clicking
%  [P1,P2] = VL_CLICKSEGMENT() lets the user select a segment in the
%  current figure and returns the starting and ending point P1 and P2.
%
%  The user can abort the operation by pressing any key. In this case
%  the function returns the empty matrix.
%
%  See also:: VL_CLICK(), VL_CLICKPOINT(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

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

