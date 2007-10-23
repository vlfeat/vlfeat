function parents = aibcut(parents, n)
% AIBCUT  Cut AIB tree
%  PARENTS = AIBCUT(PARENTS, N) cuts the AIB tree PARENTS to get N
%  words. PARENTS defines the AIB tree (see AIB()).
%
%  The N-sized cut is the set of compressed N words (clusters)
%  computed by AIB.
%
%  The function returns a vector PARENTS defining a modified AIB tree
%  in which descendents of nodes in the cut are direct children of
%  those nodes.
%
%  Nodes that where unassigned in PARENTS (i.e. equal to zero) remain
%  unassigned.
%
%  See also AIB().

% AUTORIGHTS

if n > 1
  mu = max(parents) - n + 1 ;
  stop = [find(parents(1:mu) > mu) 0] ;
else
  mu   = max(parents) ;
  stop = [mu 0] ;
end

while 1
  [drop,sel] = setdiff(parents(1:mu), stop)  ;
  sel = setdiff(sel, stop) ;
  if isempty(sel), break ; end
  parents(sel) = parents(parents(sel))  ;
end
