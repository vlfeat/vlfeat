N         = 10000 ;
dimension = 2 ;
data = rand(dimension,N) ;
numClusters = 20 ;
cc=hsv(numClusters);

[centers, assignments] = vl_kmeans(data, numClusters);

figure
hold on
for i=1:numClusters
    plot(data(1,assignments == i),data(2,assignments == i),'.','color',cc(i,:));
end
plot(centers(1,:),centers(2,:),'k.','MarkerSize',20)
axis off
vl_demo_print('kmeans_2d_rand',0.6);