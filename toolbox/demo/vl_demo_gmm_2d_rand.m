%% data init
N         = 5000 ;
dimension = 2 ;
numClusters = 20 ;
data = rand(dimension,N) ;

%% clustering
tic
vl_twister('state',0) ;
[means, sigmas, weights] = vl_gmm(data, numClusters,'MaxNumIterations',1000,'Multithreading','parallel','verbose');
toc

figure
hold on
plot(data(1,:),data(2,:),'r.');
for i=1:numClusters
    vl_plotframe([means(:,i)' sigmas(1,i) 0 sigmas(2,i)],'Color','blue','LineWidth',2);
end

title('GMM: Gaussian mixture - random init');
axis equal
set(gca,'xtick',[],'ytick',[]);
axis off
vl_demo_print('gmm_2d_rand',0.6);

