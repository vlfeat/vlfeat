function [P1,P2] = clicksegment
% CLICKSEGMENT  Select a segment by clicking
%  [P1,P2] = CLICKSEGMENT lets the user select a segment in the
%  current figure and returns the starting and ending point P1 and P2.
%
%  The user can abort the operation by pressing any key. In this
%  case the function returns the empty matrix.
%
%  See also CLICK, CLICKPOINT.


% Click first segment
P1=click ;
if isempty(P1)
  P2=P1 ;
  return ;
end

was_hold = ishold(gca) ;
hold on ;
h=plot(P1(1),P1(2),'rx') ;

% Click second segment
P2=click ;
if isempty(P2)
  P1=P2 ;
end
delete(h) ;
if ~was_hold, hold off ; end

