function [aibtree ids] = aibquantize(data, labels, K, nleaves, nclusters)

aibtree = struct();
aibtree.nclusters = nclusters;

[aibtree.tree asgn] = hikmeans(data, K, nleaves);

% Convert asgn to leaf ids
depth = size(asgn,1);
aibtree.nleaves = K^depth;

ids = uint32(zeros(1,size(asgn,2)));
offset = 1;
for i=depth:-1:1
    ids = ids + (asgn(i,:)-1)*offset;
    offset = offset * K;
end
ids = ids + 1; % go back to matlab based indexing

% build P
classes = unique(labels);
nclasses = length(classes);
P = zeros(aibtree.nleaves, nclasses);
for i = 1:nclasses
    cids = find(labels==classes(i));
    P(:,i) = histc(ids(cids), 1:aibtree.nleaves);
end

aibtree.parents = aib(P);
aibtree.map = get_map(aibtree.parents, aibtree.nleaves, nclusters);

%%%%%%%%%%%%%%%%%%%%%%%%%
% gets a mapping into the given number of clusters for all ids
function map = get_map(parents, nleaves, nclusters)

cut = get_cut_nclusters(parents, nclusters);

cut = [cut; 0]; % Add zero for nodes which have no parents
% traverse the tree to build a mapping
map = parents(1:nleaves);
nleft = nleaves;
while nleft ~= 0
   [left idx] = setdiff(map, cut);
   nleft = length(left);
   % Move up the tree
   map(idx) = parents(map(idx));
end 

ids = unique(map);
for i = 1:length(ids)
    map(find(map==ids(i))) = i;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%
% cuts a set of leaves whose parents are >= cut but who themselves are < cut
function sel = get_cut_nclusters(parents, nclusters)
cut  = (length(parents)+1-nclusters);
sel  = find(parents > cut) ;
sel  = sel(sel <= cut) ;

