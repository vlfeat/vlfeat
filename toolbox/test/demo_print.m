function demo_print(name)
% DEMO_PRINT
%   Print current figure to the documentation directory.

pfx = fullfile(vlfeat_root,'doc','figures','demo',[name '.eps']) ;
print('-depsc',pfx) ;
