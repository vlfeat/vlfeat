function test_ikmeans
% TEST_IKMEANS Test IKMEANS function

fprintf('test_ikmeans: Testing IKMEANS and IKMEANSPUSH\n')

% -----------------------------------------------------------------------
fprintf('test_ikmeans: Testing Lloyd algorithm\n')

K       = 3 ;
data    = uint8(rand(2,1000) * 255) ;
datat   = uint8(rand(2,10000)* 255) ;

[C,A] = ikmeans(data,K,'verbose') ;
[AT]  = ikmeanspush(datat,C,'verbose') ;

figure(1) ; clf ; hold on ;
plot_partition(data, datat, C, A, AT) ;
title('ikmeans (Lloyd algorithm)') ;
demo_print('ikmeans_lloyd') ;

% -----------------------------------------------------------------------
fprintf('test_ikmeans: Testing Elkan algorithm\n')

[C,A] = ikmeans(data,K,'verbose','method','elkan') ;
[AT]  = ikmeanspush(datat,C,'verbose','method','elkan') ;

figure(2) ; clf ; hold on ;
plot_partition(data, datat, C, A, AT) ;
title('ikmeans (Elkan algorithm)') ;
demo_print('ikmeans_elkan') ;

% -----------------------------------------------------------------------
function plot_partition(data, datat, C, A, AT)
% -----------------------------------------------------------------------

K = size(C,2) ;
cl = get(gca,'ColorOrder') ;
ncl = size(cl,1) ;
for k=1:K
  sel  = find(A  == k) ;
  selt = find(AT == k) ;
  plotframe(data(:,sel),  '.','Color',cl(mod(k,ncl)+1,:)) ;
  plotframe(datat(:,selt),'+','Color',cl(mod(k,ncl)+1,:)) ;  
end
plot(C(1,:),C(2,:),'ko','markersize',10','linewidth',6) ;
plot(C(1,:),C(2,:),'yo','markersize',10','linewidth',1) ;

axis off ; axis equal ;
