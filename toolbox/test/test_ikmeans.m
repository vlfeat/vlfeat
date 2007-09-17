% TEST_IKMEANS Test IKMEANS function

K=3;
[u,v]=meshgrid(0:5:255,0:5:255) ;
X = uint8([u(:)';v(:)']) ;
perm=randperm(size(X,2));
Y = X(:,perm(51:end)) ;
X = X(:,perm(1:51)) ;
[C,L]=ikmeans(X,K) ;
Q=ikmeanspush(Y,C) ;

figure(100) ; clf ; set(gcf,'color','w') ;
cl = {'r','g','b','k','y'} ;
for k=1:K
  s = find(L==k) ;
  q = find(Q==k) ; 
  plotframe(X(:,s),'linestyle','*','color',cl{k}) ;  hold on ;  
  plotframe(X(:,s),'linestyle','o','color','k') ;
  plotframe(Y(:,q),'linestyle','.','color',cl{k}) ;
end
plotframe(C,'color','k','linestyle','o','markersize',10,'linewidth',5) ;
plotframe(C,'color','y','linestyle','o','markersize',10) ;
title('ikmeans partitions') ;
axis equal ; axis tight ; axis off ;
