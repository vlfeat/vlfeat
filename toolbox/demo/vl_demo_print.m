function vl_demo_print(name,r)
% VL_DEMO_PRINT
%   Print current figure to the documentation directory.

if nargin > 1
  set(gcf,'paperunits','normalized') ;
  pos = get(gcf,'paperposition') ;
  s = r/pos(3) ;
  set(gcf,'paperposition',s*pos) ;
end

pfx = fullfile(vl_root,'doc','demo') ;
if ~ exist(pfx, 'dir')
  mkdir(pfx) ;
end

filename = fullfile(pfx, [name '.eps']) ;
print('-depsc', filename) ;
fprintf('demo_print: wrote file ''%s''\n', filename) ;
