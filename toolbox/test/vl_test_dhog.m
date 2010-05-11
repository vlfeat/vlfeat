function vl_test_dhog
% VL_TEST_DHOG Test VL_DHOG function
%   For a correctness test, see TEST_DHOG_INTER.

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

tri = 3 ;
I = single(vl_test_pattern(2))  ;
n = 1 ;

for siz=1:5
  for step=1:10
    ben(n).size = siz ;
    ben(n).step = step ;
    tic ;
    for t=1:tri
      [f_,d_] = vl_dhog(I,'step', step, 'size', siz) ;
    end
    ben(n).elaps_full = toc / tri ;

    tic ;
    for t=1:tri
      [f_,d_] = vl_dhog(I,'step', step, 'size', siz, 'fast') ;
    end
    ben(n).elaps_fast = toc / tri ;
    n=n+1 ;
  end
end

fprintf('%5s %5s %10s %10s %9s\n', 'siz','step','full','fast','speedup') ;
for b=ben
  fprintf('%5d %5d %8.1fus %8.1fus %8.1fX\n', ...
          b.size, b.step, 1e3*b.elaps_full, 1e3*b.elaps_fast, ...
          b.elaps_full/b.elaps_fast);
end
