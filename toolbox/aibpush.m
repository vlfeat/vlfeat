function ids = aibpush(aibtree, data)

% Push down the hkm tree
asgn = hikmeanspush(aibtree.tree, data);

% Convert asgn to leaf ids
K = size(aibtree.tree.centers,2);
depth = size(asgn,1);
ids = uint32(zeros(1,size(asgn,2)));
offset = 1;
for i=depth:-1:1
    ids = ids + (asgn(i,:)-1)*offset;
    offset = offset * K;
end
ids = ids + 1; % go back to matlab based indexing

% Map hikmeans ids to aib ids
ids = aibtree.map(ids);
