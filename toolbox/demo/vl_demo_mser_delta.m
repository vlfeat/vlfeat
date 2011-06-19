% VL_DEMO_MSER_DELTA  Demo: MSER: delta parameter

randn('state',0) ;
rand('state',0) ;

I = zeros(100,500) + 255 ;
for i=1:5
  I((1:50) + 25 - 1, ...
    (1:50) + 25 - 1 + 100 * (i-1)) = ...
    255 - 32 * i ;
end
I = uint8(I) ;

figure(1) ; clf ;
axes('position',[0.05 0.05 1-.1 2*1/5-.1]) ;
plot(I(end/2,:),'linewidth',3) ;
hold on ;
vl_demo_print('mser_delta_0') ;


figure(2) ; clf ;
imagesc(I) ; axis off ; axis equal ; axis tight ;
colormap(gray(256)) ;
set(gca,'fontsize',20) ;

deltar = [1 32 159 160] ;
clear h ;
for delta=deltar
  [r,f] = vl_mser(I, 'Delta', delta, 'BrightOnDark', 0, 'verbose') ;

  if exist('h','var'), delete(h) ; end
  h = vl_plotframe(vl_ertr(f)) ;
  if ~isempty(h), set(h,'color','y','linewidth',1) ; end
  title(sprintf('delta = %g', delta)) ;
  drawnow ;

  vl_demo_print(sprintf('mser_delta_%d',find(delta==deltar))) ;
end
