function vl_demo_ikmeans()
% VL_DEMO_IKMEANS

numData = 10000 ;
dimension = 2 ;
data = uint8(255*rand(dimension,numData)) ;
numClusters = 3^3 ;

[centers, assignments] = vl_ikmeans(data, numClusters);

figure(1) ; clf ; axis off ;
plotClusters(data, centers, assignments) ;
vl_demo_print('ikmeans_2d',0.6);

[tree, assignments] = vl_hikmeans(data,3,numClusters) ;
figure(2) ; clf ; axis off ;
plotClusters(data, [], [4 2 1] * double(assignments)) ;
vl_demo_print('hikmeans_2d',0.6);

function plotClusters(data, centers, assignments)
hold on ;
cc=jet(double(max(assignments(:))));
for i=1:max(assignments(:))
    plot(data(1,assignments == i),data(2,assignments == i),'.','color',cc(i,:));
end
if ~isempty(centers)
  plot(centers(1,:),centers(2,:),'k.','MarkerSize',20)
end
