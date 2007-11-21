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
%  Node that in PARENTS are conneted to the null node may be added to
%  the cut if no enugh proper nodes are sufficient to fill the
%  requested cut.
%
%  [CUT, MAP] = AIBCUT(...) returns also a vector MAP which
%  short-circuits nodes below the cut to nodes of the cut.  Nodes
%  above or in the cut are short-circutited to themselves.  Nodes that
%  where connected to the null node are short-circuited to zero.
%  Nodes that where connected to the null but are also in the cut are
%  short circutied to themselves.
%
%  See also AIB(), AIBHIST(), AIBCUTHIST().

% AUTORIGHTS


if n > 1
  root    = max(parents) ;
  
  % count number of null nodes
  z = sum(parents(1:root) == 0) ;
  
  % determine number of leves
  nleaves = (root - z + 1) / 2 ;
  
  % find first node of the cut
  mu   = root - min(n, nleaves) + 1 ;
    
  % correction for presence of null nodes
  nz   = find(parents(1:mu) > 0) ;
  mu   = nz(end) ;
  
  % find node belnoging to the cut
  cut  = find(parents(1:mu) > mu) ;
  
  % In the presence of null nodes, the cut size might exceed
  % nleaves, which is the maximum cut size we can obtain with the
  % specified tree. The additional nodes have to be picked up from
  % the null nodes.

  if length(cut) < n
    sel_z = find(parents == 0) ;
    cut = [sel_z(1:n-length(cut)) cut] ;
  end
  
  % aesthetic reasons only
  cut = sort(cut) ;
  
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
  
  map(setdiff(find(parents == 0), cut)) = 0 ;
end