function test_mser(n)
% TEST_MSER

if nargin < 1
  n = 1 ;
end

I     = test_pattern(n) ;
I     = uint8(I) ; 
[M,N] = size(I) ;

imwrite(I,'../results/autotest.pgm') ;

[p,f] = mser(I,...
             'delta',6,...
             'epsilon',.3,...
             'maxarea',1.0,...
             'minarea',0.0,...             
             'maxvariation',0.3,...
             'dups',...
             'verbose') ;

figure(100) ; clf ;  
colormap gray ;

subplot(1,2,1) ; 
imagesc(I) ; 
hold on ;
[i,j] = ind2sub([M,N],p) ;
plot(j,i,'r*') ;
m=zeros(size(I)) ;

subplot(1,2,2) ;
for i=p'
  sel = erfill(I,i) ;
  m(sel) = m(sel)+1 ;
end
f= f([2 1 5 4 3],:) ;

imagesc(m) ;
hold on ; 
plotframe(f) ;
