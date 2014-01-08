function f = vl_lbpfliplr(f)
% VL_LBPFLIPLR  Flip LBP features left-right
%   F = VL_LBPFLIPLR(F) flips the LBP features F from left to
%   right.
%
%   Example::
%     The following instructions return results almost identical
%     assuming that the width of I is exactly divisible by the cell
%     size:
%       F = VL_LBP(FLIPLR(I), 8)
%       F_ = VL_LBPFLIPLR(VL_LBP(I), 8)
%
%   See also: VL_LBP(), VL_HELP().

% Copyright (C) 2013 Andrea Vedaldi.
% Copyright (C) 2010-11 Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% Uniform patterns for a 8-neighbourhood: patterns 58 (generic) and 57
% (constant) are invariant to an image warp.

perm = zeros(1,58) ;
perm(58) = 58 ;
perm(57) = 57 ;

% i is the start of the sequence of 1 and j its length
% recall that i=0 -> E, i=1 -> SE, etc
% the length j remains the same.
% flipping the start gives the flipped end, and subtracting the length
% gives the flipped start

for i = 0:7
  for j = 1:7
    i_ = mod(4 - i - j + 1, 8) ;
    j_ = j ;
    perm(i * 7 + j) = i_ * 7 + j_ ;
  end
end

f = f(:, end:-1:1, perm) ;
