function [cut, map, short] = vl_aibcut(parents, n)
% VL_AIBCUT  Cut VL_AIB tree
%  CUT = VL_AIBCUT(PARENTS, N) cuts the binary merge tree PARENTS and
%  returns a cut CUT of N nodes. The format of PARENTS is the same
%  used by the VL_AIB() function.
%
%  A cut is a set of N nodes such that no node is a descendant of any
%  other node in the cut and such that all leaves descend from a node
%  in the cut. The vector CUT lists the nodes of the binary merge tree
%  PARENT that form the cut.
%
%  Nodes with null parent (as defined by PARENTS) are included in the
%  cut if the other nodes are not enough to fill a cut of N elements.
%
%  [CUT, MAP] = VL_AIBCUT(...) returns a vector MAP with the same size
%  as PARENTS. MAP assigns each node below or in the cut to the
%  corresponding element in the CUT vector (each element above the cut
%  or with null parent is mapped to 0). To get the index of the
%  corresponding cut nodes use CUT(MAP). MAP can be used to quantize
%  the leaves in a sequences of N contiguous indexes, starting from
%  one (see also VL_AIBCUTPUSH()).
%
%  [CUT, MAP, SHORT] = VL_AIBCUT(...) returns also a vector SHORT that
%  represents a version of the PARENTS tree where nodes below the cut
%  are short-circuitied to link to the corresponding cut ancestors
%  directly. Null parents are left unchanged, except if the
%  corresponding node is in the cut (in which case the map-to-itself
%  rule has the precedence).
%
%  See also: VL_HELP(), VL_AIB().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% --------------------------------------------------------------------
%                                           Determine nodes in the cut
% --------------------------------------------------------------------

if n > 1
  root = max(parents) ;

  % count number of null nodes
  z = sum(parents(1:root) == 0) ;

  % determine number of leves
  nleaves = (root - z + 1) / 2 ;

  % find first node of the cut
  mu = root - min(n, nleaves) + 1 ;

  % correction for presence of null nodes
  nz = find(parents(1:mu) > 0) ;
  mu = nz(end) ;

  % find node belnoging to the cut
  cut = find(parents(1:mu) > mu) ;

  % In the presence of null nodes, the cut size might exceed nleaves,
  % which is the maximum cut size we can obtain with the specified
  % tree. The additional nodes have to be picked up from the null
  % nodes.

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
