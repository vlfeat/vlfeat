sz = 5 ;
st = 5 ;
I = single(test_pattern(2))  ;
%I = I + 100*single(randn(size(I))) ;
[f,d] = dhog(I,'step', st, 'size', sz, 'verbose') ;% 'fast') ;
[f,d] = dhog(I,'step', st, 'size', sz, 'verbose', 'fast') ;

f = [f  ; sz * ones(1,size(f,2)) ; pi/2 - zeros(1,size(f,2))] ;

figure(1) ; clf ; 
imagesc(I) ; hold on ; colormap gray ; 
h = [] ;
while 1
  p = click ;
  dst = (f(1,:) - p(1)).^2 + (f(2,:) - p(2)).^2 ;
  [dst, i] = min(dst) ;
  if ~isempty(h)
    delete(h) ;
  end
  h = plotsiftdescriptor(d(:,i),f(:,i),'magnif',1) ;
end



