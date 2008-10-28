function vl_test_ikmeans
% VL_TEST_IKMEANS Test VL_IKMEANS function

fprintf('test_ikmeans: Testing VL_IKMEANS and IKMEANSPUSH\n')

% -----------------------------------------------------------------------
fprintf('test_ikmeans: Testing Lloyd algorithm\n')

K       = 3 ;
data    = uint8(rand(2,1000) * 255) ;
datat   = uint8(rand(2,10000)* 255) ;

[C,A] = vl_ikmeans(data,K,'verbose') ;
[AT]  = vl_ikmeanspush(datat,C,'verbose') ;

figure(1) ; clf ; hold on ;
plot_partition(data, datat, C, A, AT) ;
title('vl_ikmeans (Lloyd algorithm)') ;
vl_demo_print('ikmeans_lloyd') ;

% -----------------------------------------------------------------------
fprintf('test_ikmeans: Testing Elkan algorithm\n')

[C,A] = vl_ikmeans(data,K,'verbose','method','elkan') ;
[AT]  = vl_ikmeanspush(datat,C,'verbose','method','elkan') ;

figure(2) ; clf ; hold on ;
plot_partition(data, datat, C, A, AT) ;
title('vl_ikmeans (Elkan algorithm)') ;
vl_demo_print('ikmeans_elkan') ;

% -----------------------------------------------------------------------
function plot_partition(data, datat, C, A, AT)
% -----------------------------------------------------------------------

K = size(C,2) ;
cl = get(gca,'ColorOrder') ;
ncl = size(cl,1) ;
for k=1:K
  sel  = find(A  == k) ;
  selt = find(AT == k) ;
  vl_plotframe(data(:,sel),  '.','Color',cl(mod(k,ncl)+1,:)) ;
  vl_plotframe(datat(:,selt),'+','Color',cl(mod(k,ncl)+1,:)) ;  
end
plot(C(1,:),C(2,:),'ko','markersize',10','linewidth',6) ;
plot(C(1,:),C(2,:),'yo','markersize',10','linewidth',1) ;

axis off ; axis equal ;
