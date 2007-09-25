% TEST_IKMEANS Test IKMEANS function

K       = 3 ;
data    = uint8(rand(2,1000) * 255) ;
datat   = uint8(rand(2,10000)* 255) ;

[C,A] = ikmeans(data,K) ;
[AT]  = ikmeanspush(datat,C) ;

figure(1) ; clf ; hold on ;

cl = get(gca,'ColorOrder') ;
ncl = size(cl,1) ;
for k=1:K
  sel  = find(A  == k) ;
  selt = find(AT == k) ;
  plot(data(1,sel),  data(2,sel),  '.','Color',cl(mod(k,ncl)+1,:)) ;
  plot(datat(1,selt),datat(2,selt),'+','Color',cl(mod(k,ncl)+1,:)) ;  
end
plot(C(1,:),C(2,:),'ko','markersize',10','linewidth',6) ;
plot(C(1,:),C(2,:),'yo','markersize',10','linewidth',1) ;

axis off ; axis equal ;
demo_print('ikmeans') ;
