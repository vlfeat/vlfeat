function [cut, map] = aibcut(parents, n)
% AIBCUT  Cut AIB tree
%  CUT = AIBCUT(PARENTS, N) cuts the AIB tree PARENTS to get a cut of
%  N nodes. PARENTS defines the AIB tree (see AIB()).
%
%  The N-sized cut is a compressed (coarser) partition of the original
%  clusters as computed by AIB.
%
%  The function returns a vector CUT with the list of nodes of the
%  AIB tree belonging to the cut.
%
%  [CUT, MAP] = AIBCUT(...) returns also a vector MAP which
%  short-circuits nodesa below the cut to nodes of the cut.
%  Nodes above or in the cut are short-circutited to themselves.
%  Nodes that where connected to the null node are short-circuited
%  to zero.
%
%  See also AIB(), AIBHIST(), AIBCUTHIST().

% AUTORIGHTS


%
%  The function returns a vector CUT that short-circuits nodes of the
%  AIB tree to their parent in the cut. Nodes in or above the cut are
%  short-cricutited to themselves. Nodes with null parent are
%  short-circuitied to zero.
%
%  See also AIB().

% AUTORIGHTS

if n > 1
  mu   = max(parents) - n + 1 ;
  cut  = find(parents(1:mu) > mu) ;
else
  mu   = max(parents) ;
  cut  = mu ;
end

if nargout > 1
  stop = [cut find(parents == 0)] ;
  map = 1:length(parents) ;
  
  while 1
    [drop,sel] = setdiff(map(1:mu), stop)  ;
    sel = setdiff(sel, stop) ;
    if isempty(sel), break ; end
    map(sel) = parents(map(sel))  ;
  end
  
  map(find(parents == 0)) = 0 ;
end