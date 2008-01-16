function demo_print(name,r)
% DEMO_PRINT
%   Print current figure to the documentation directory.

if nargin > 1
  set(gcf,'paperunits','normalized') ;
  pos = get(gcf,'paperposition') ;
  s = r/pos(3) ;
  set(gcf,'paperposition',s*pos) ;
end

pfx = fullfile(vlfeat_root,'doc','figures','demo',[name '.eps']) ;
print('-depsc',pfx) ;
fprintf('Printed picture ''%s''\n', pfx)
