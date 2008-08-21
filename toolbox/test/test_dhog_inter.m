siz = 4 ;
step = 10 ;
I = single(test_pattern(2))  ;
[f,d] = dhog(I,'step', step, 'size', siz) ;
[f_,d_] = dhog(I,'step', step, 'size', siz, 'fast') ;

[Ix, Iy]  = grad(I) ;
mod       = sqrt(Ix.^2 + Iy.^2) ;
ang       = atan2(Iy,Ix) ;
grd       = shiftdim(cat(3,mod,ang),2) ;
grd       = single(grd) ;

f = [f  ; siz * ones(1,size(f,2)) / 3 ; zeros(1,size(f,2))] ;
d__ = [] ;
for f1=f 
  d__ = [d__ siftdescriptor(grd, f1)] ;
end

figure(1) ; clf ; 
imagesc(I) ; hold on ; colormap gray ; 
title('blue:dhog, red:fast dhog, green:sift') ;
h = [] ;
while 1
  p = click ;
  dst = (f(1,:) - p(1)).^2 + (f(2,:) - p(2)).^2 ;
  [dst, i] = min(dst) ;
  if ~isempty(h)
    delete(h) ;
  end
  h   = plotsiftdescriptor(d(:,i),f(:,i)) ;
  h_  = plotsiftdescriptor(d_(:,i),f(:,i)) ;
  h__ = plotsiftdescriptor(d__(:,i),f(:,i)) ;
  set(h,'color','b') ;
  set(h_,'color','r') ;
  set(h__,'color','g') ;
  set(h,'linewidth',4) ;
  set(h_,'linewidth',2) ;
end
