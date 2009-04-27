% VL_FLATMAP Flatten a tree, assigning the label of the root to each node
%   [LABELS CLUSTERS] = VL_FLATMAP(MAP) labels each tree of the forest contained
%   in MAP. LABELS contains the linear index of the root node in MAP, CLUSTERS
%   instead contains a label between 1 and the number of clusters.
function [map, C] = vl_flatmap(map)

% follow the parents list to the root nodes (where nothing changes)
while 1
  map_ = map(map) ;
  if isequal(map_,map) ; break ; end
  map = map_ ;  
end

[drop,drop,C] = unique(map)  ;
