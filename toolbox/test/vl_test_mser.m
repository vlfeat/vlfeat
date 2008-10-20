function vl_test_mser(n)
% VL_TEST_MSER

if nargin < 1
  n = 1 ;
end

I     = vl_test_pattern(n) ;
I     = uint8(I) ; 
[M,N] = size(I) ;

pfx = fullfile(vlfeat_root,'results') ;
if ~ exist(pfx, 'dir')
  mkdir(pfx) ;
end

imwrite(I,fullfile(pfx, 'autotest.pgm')) ;

[p,f] = vl_mser(I,...
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
  sel = vl_erfill(I,i) ;
  m(sel) = m(sel)+1 ;
end
f= f([2 1 5 4 3],:) ;

imagesc(m) ;
hold on ; 
vl_plotframe(f) ;
