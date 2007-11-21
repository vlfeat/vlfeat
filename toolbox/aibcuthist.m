function hist = aibcuthist(cut, map, x)
% AIBCUTHIST  Compute histogram over AIB cut
%  H = AIBCUTHIST(CUT, MAP, X) computes the histogram of the data X
%  over the specified AIB cut. CUT and MAP define (as returned by
%  AIBCUT()) define the cut. Each element of hist counts how many
%  elements of X are projected in the corresponding cut node.
%
%  X must contain nodes that are below or in the cut (normally
%  leaves).
%
%  If the original AIB tree cointained a null node, then some of
%  the data might be fall off the cut and be projected to this
%  node. In this case, an extra bin is appendend at the end of HIST
%  to count for such occurences.

% AUTORIGHTS

% enumerate cut nodes
cut_size = length(cut) ;
colors = zeros(size(map)) ;
colors(cut) = 1:cut_size ;

% assign a color to all nodes of the map except the null ones
sel_nz = find(map > 0) ;
colors(sel_nz) = colors(map(sel_nz)) ;

% assign a color to the null ones too
sel_z = find(map == 0) ;
if ~isempty(sel_z)
  cut_size = cut_size + 1 ;
  colors(sel_z) = cut_size ;
end

% Now we have the nodes of the cut. Accumulate.
hist = zeros(1, cut_size) ;
hist = binsum(hist, ones(size(x)), colors(x)) ;

