%% data init
N         = 5000 ;
dimension = 2 ;
data = rand(dimension,N) ;

numClusters = 30 ;
[means, sigmas, weights] = vl_gmm(data, numClusters);

figure
hold on
plot(data(1,:),data(2,:),'r.');
for i=1:numClusters
    vl_plotframe([means(:,i)' sigmas(1,i) 0 sigmas(2,i)]);
end

title('GMM: Gaussian mixture - random init');
axis equal
set(gca,'xtick',[],'ytick',[]);
axis off
vl_demo_print('gmm_2d_rand',0.6);

