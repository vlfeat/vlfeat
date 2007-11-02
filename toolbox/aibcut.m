function cut = aibcut(parents, n)
% AIBCUT  Cut AIB tree
%  CUT = AIBCUT(PARENTS, N) cuts the AIB tree PARENTS to get N
%  words. PARENTS defines the AIB tree (see AIB()).
%
%  The N-sized cut is the set of compressed N words (clusters)
%  computed by AIB.
%
%  The function returns a vector CUT that short-circuits nodes of the
%  AIB tree to their parent in the CUT. Nodes in or above the cut are
%  short-cricutited to themselves. Nodes with null parent are
%  short-circuitied to zero.
%
%  See also AIB().

% AUTORIGHTS

if n > 1
  mu = max(parents) - n + 1 ;
  stop = [find(parents(1:mu) > mu) find(parents == 0)] ;
else
  mu   = max(parents) ;
  stop = [mu find(parents == 0)] ;
end

cut = 1:length(parents) ;

while 1
  [drop,sel] = setdiff(cut(1:mu), stop)  ;
  sel = setdiff(sel, stop) ;
  if isempty(sel), break ; end
  cut(sel) = parents(cut(sel))  ;
end

cut(find(parents == 0)) = 0 ;
