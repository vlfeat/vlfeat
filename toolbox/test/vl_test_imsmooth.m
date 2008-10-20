function vl_test_imsmooth

I = im2double(imread('data/spots.jpg')) ;
I = max(min(imresize(I,2),1),0) ;
I = single(I) ;

global fign ;
fign = 1 ;
step = 1 ;
ker  = 'gaussian' ;

testmany(I,'triangular',1) ;
testmany(I,'triangular',2) ;

testmany(I,'gaussian',1) ;
testmany(I,'gaussian',2) ;

function testmany(I,ker,step)
global fign ;
sigmar = [0, 1, 10, 100] ;
for sigma = sigmar
  [I1,I2,I3] = testone(I,ker,sigma,step) ;
  compare(fign,I1,I2,I3,sprintf('%s, sigma %g, sub. step %d', ker, sigma, step)) ;
  fign=fign+1 ;
end

function I=icut(I)
I=min(max(I,0),1) ;

function [I1,I2,I3]=testone(I,ker,sigma,step)
switch ker
  case 'gaussian'
    W = ceil(4*sigma) ;
    g = exp(-.5*((-W:W)/(sigma+eps)).^2) ;
  case 'triangular'
    W = max(round(sigma),1) ;
    g = W - abs(-W+1:W-1) ;
end
g = g / sum(g) ;

I1 = imconv(I,g) ;
I1 = I1(1:step:end,1:step:end,:) ;
I2 = vl_imsmooth(I,sigma,'kernel',ker,'padding','zero',      'verbose','subsample',step) ;
I3 = vl_imsmooth(I,sigma,'kernel',ker,'padding','continuity','verbose','subsample',step) ;

function compare(n,I1,I2,I3,tit)
figure(n) ; clf ; colormap gray ;
subplot(1,3,1) ; axis equal ; imagesc(icut(I1)) ; axis off ; 
title('Matlab zeropad') ;
subplot(1,3,2) ; axis equal ; imagesc(abs(I1-I2)) ; axis off ; 
title('matlab - imsmooth') ;
subplot(1,3,3) ; axis equal ; imagesc(icut(I3)) ; axis off ; 
title('vl_imsmooth contpad') ;
set(n,'name',tit) ;

function I=imconv(I,g)
if strcmp(class(I),'single')
  g = single(g) ;
else
  g = double(g) ;
end
for k=1:size(I,3) 
  I(:,:,k) = conv2(g,g,I(:,:,k),'same'); 
end
