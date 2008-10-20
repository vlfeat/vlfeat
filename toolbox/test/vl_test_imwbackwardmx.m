% VL_TEST_IMWBACKWARDMX  Test: imwbackwardmx

I = vl_test_pattern(102) ;

figure(1) ; clf ; 
imagesc(I) ;
colormap(gray(256)) ;
axis equal ; axis off ;

[M,N] = size(I) ;
ur    = linspace(-N/2, N/2, N) ;
vr    = linspace(-M/2, M/2, M) ;
[u,v] = meshgrid(ur,vr) ;

for s=.75*(1+cos(linspace(0,2*pi,100)))/2+.25
  up = (cos(s-1)*u - sin(s-1)*v) / s ;
  vp = (sin(s-1)*u + cos(s-1)*v) / s ;
  J = vl_imwbackward(ur,vr,vl_imsmooth(I,1/s*.5),up,vp) ;
  imagesc(J) ; drawnow ; 
end
