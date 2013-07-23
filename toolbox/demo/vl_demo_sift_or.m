% VL_DEMO_SIFT_OR  Demonstrates SIFT orientation detection

I = vl_impattern('wedge') ;
ur = 1:size(I,2) ;
vr = 1:size(I,1) ;

% distribute frames on a grid
[u,v] = meshgrid(ur(5:10:end-4),vr(5:10:end-4)) ;
f = [u(:)';v(:)'] ;
K = size(f,2) ;
f = [f ; 4 * ones(1,K) ; 0 * ones(1,K)] ;

% detect orienntations
f = vl_sift(single(I), 'frames', f, 'orientations') ;

figure(1) ; clf ;
imagesc(single(I)) ; colormap gray ; hold on ;
vl_plotframe(f,'color','k','linewidth',3) ;
vl_plotframe(f,'color','y','linewidth',2) ;
axis equal ; axis off ;
vl_demo_print('sift_or') ;
