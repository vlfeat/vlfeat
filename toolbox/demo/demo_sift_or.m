I = test_pattern(1) ;
ur = 1:size(I,2) ;
vr = 1:size(I,1) ;

[u,v] = meshgrid(ur(1:5:end),vr(1:5:end)) ;

f = [u(:)';v(:)'] ;
K = size(f,2) ;
f = [f ; 2 * ones(1,K) ; 0 * ones(1,K)] ;

f = sift(single(I), 'frames', f, 'orientations') ;

%f = diag([1 1 6 1]) * f ;

figure(1) ; clf ;
imagesc(I) ; colormap gray ; hold on ;
plotframe(f,'color','y','linewidth',3) ;
plotframe(f,'color','k','linewidth',1) ; axis equal ; axis off ;

demo_print('sift_or') ;



