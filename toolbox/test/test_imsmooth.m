I = im2double(imread('data/spots.jpg')) ;
I = max(min(imresize(I,2),1),0) ;
I = im2single(I) ;
J = imsmooth(I, 1, 'padding', 'zero', 'verbose') ;
figure(2) ; clf ;
imagesc([I J]) ;

I = im2single(I) ;
J = imsmooth(I, 1, 'padding', 'continuity', 'verbose') ;
figure(3) ; clf ;
imagesc([I J]) ;

J = imsmooth(I, 1, 'subsample', 2, 'verbose') ;
figure(4) ; clf ;
imagesc(J) ;

tim=[] ;
for sigma=logspace(-2,2,100) 
  tic ; I_ = imsmooth(I, sigma) ; tim = [tim toc] ;
  figure(5) ; subplot(1,2,1) ;
  imagesc(max(0,min(1,I_))) ; 
  subplot(1,2,2) ; plot(tim) ;
  drawnow ;
end

%I=im2double(I);
%tic ;
%for t=1:10000
%  imsmooth(I, 1, 'nosimd') ;
%end
%toc ;

%tic ;
%for t=1:10000
%  imsmooth(I, 1) ;
%end
%toc ;


