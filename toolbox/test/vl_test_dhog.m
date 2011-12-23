function vl_test_dhog
% VL_TEST_DHOG Test VL_DHOG function
%   For a correctness test, see TEST_DHOG_INTER.

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

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
