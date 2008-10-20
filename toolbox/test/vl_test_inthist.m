L = uint32(round(10 * rand(10,20))) ;

H = inthist(L) ;

figure(1) ; clf ;
subplot(1,2,1) ;
imagesc(L) ;
subplot(1,2,2) ;
vl_imarraysc(H) ;


H = inthist(L, 'numlabels', 16) ;

figure(2) ; clf ;
subplot(1,2,1) ;
imagesc(L) ;
subplot(1,2,2) ;
vl_imarraysc(H) ;

W = rand(size(L)) .* (L > 0) ;
H = inthist(L, 'mass', W, 'verbose') ;

figure(3) ; clf ;
subplot(1,3,1) ;
imagesc(L) ;
subplot(1,3,2) ;
imagesc(W) ;
subplot(1,3,3) ;
vl_imarraysc(H) ;


W = uint32(10*rand(size(L)) .* (L > 0)) ;
H = inthist(L, 'mass', W, 'verbose') ;

figure(4) ; clf ;
subplot(1,3,1) ;
imagesc(L) ;
subplot(1,3,2) ;
imagesc(W) ;
subplot(1,3,3) ;
vl_imarraysc(H) ;


boxes = 10 * rand(4,20) + .5 ;
boxes(3:4,:) = boxes(3:4,:) + boxes(1:2,:) ;
boxes = min(boxes, 10) ;
boxes = uint32(boxes) ;

h = samplinthist(H, boxes) ;



