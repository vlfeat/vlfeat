% VL_DEMO_SIFT_EDGE  Demo: SIFT: edge treshold

randn('state',0) ;
rand('state',0) ;

I = zeros(100,500) ;
for i=[10 20 30 40 50 60 70 80 90]
  d = round(i/3) ;
  I(50-d:50+d,i*5) = 1 ;
end
I = 2*pi*8^2 * vl_imsmooth(I,8) ;
I = single(255 * I) ;

figure(1) ; clf ;
imagesc(I) ; colormap gray ;
axis equal ;  axis off ; axis tight ;
hold on ;

vl_demo_print('sift_edge_0') ;

ter=[3.5 5 7.5 10] ;
for te=ter
  f = vl_sift(I, ...
           'PeakThresh',   0,  ...
           'EdgeThresh',   te, ...
           'FirstOctave', -1  ) ;
  h1 = vl_plotframe(f) ;
  set(h1,'color','k','linewidth',3) ;
  h2 = vl_plotframe(f) ;
  set(h2,'color','y','linewidth',2) ;

  vl_demo_print(sprintf('sift_edge_%d', find(te==ter))) ;
  delete(h1) ;
  delete(h2) ;
end
