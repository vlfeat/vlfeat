function [cut, map, short] = aibcut(parents, n)
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
%  Nodes with null parent (as defined by PARENTS) are comprised in the
%  cut if the other nodes are not enough to fill a cut of N elements.
%
%  [CUT, MAP] = AIBCUT(...) returns a vector MAP with the same size as
%  PARENTS. Each element of the map re-assign each node below or in
%  the cut to the corresponding CUT element and each element above the
%  cut or with null parent to 0. MAP can be used to re-quantize AIB
%  leaves in one step, or by means of AIBCUTPUSH() (which also deals
%  with nodes with null parent).
%
%  [CUT, MAP, SHORT] = AIBCUT(...) returns also a vector SHORT which
%  short-circuits nodes below the cut to nodes of the cut.  Nodes
%  above or in the cut are short-circutited to themselves.  Nodes that
%  where connected to the null node are short-circuited to zero.
%  Nodes that where connected to the null but are also in the cut are
%  short circutied to themselves.
%
%  See also AIB(), AIBHIST(), AIBCUTHIST().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

% --------------------------------------------------------------------
%                                           Determine nodes in the cut
% --------------------------------------------------------------------

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

% --------------------------------------------------------------------
%                                       Short-circuit nodes to the cut
% --------------------------------------------------------------------

stop = [cut find(parents == 0)] ;
short = 1:length(parents) ;

while 1
  [drop,sel] = setdiff(short(1:mu), stop)  ;
  sel = setdiff(sel, stop) ;
  if isempty(sel), break ; end
  short(sel) = parents(short(sel))  ;
end

short(setdiff(find(parents == 0), cut)) = 0 ;

% --------------------------------------------------------------------
%                                                  Build quantizer map
% --------------------------------------------------------------------

map             = 1:numel(parents) ;
map(cut)        = 1:n ;
map(short >  0) = map(short(short > 0)) ;
map(short == 0) = 0 ; 
map(mu+1:end)   = 0 ;
