% VL_IHASHSUM  Accumulate integer labels into a hash table
%   [H,ID,NEXT] = VL_IHASHSUM(H,ID,NEXT,K,X) counts the number of
%   occurences of the columns of X, accumulating these to the hash
%   table represented by the tripled H,ID,NEXT.
%
%   X is a D x N array of class UINT8 each row of which defines an D
%   dimensional label. Labels cannot be all zeros.
%
%   H and NEXT are 1 x C arrays of class UINT32 and ID is a D x C
%   array of class UINT8. H is a vector of counts, ID stores, for each
%   element of H, the corresponding label, and NEXT is a vector of
%   indexes.
%
%   Once constructed, the hash table can be searched by means of the
%   VL_IHASHFIND() function.
%
%   The hash table uses double hashing [1] with an initial size equal
%   to K (so that C >= K). Given a label X, this is first hashed by
%   using the FNV algorithm [2] to one of K bucket. If this bucket is
%   free, it is assigned to label X and the count is incremented.  If
%   the bucket is already assigned to the same label X, the count is
%   incremented. If the bucket is already assigned to a different
%   label, a second hash is used to scan (probe) the table for a free
%   bucket.
%
%   If no free/matching bucket is found (because the hash table is
%   full) an overflow area containing extra buckets is used. This is
%   visited by reading off indexe from the NEXT vector, until a
%   matching bucket is found or the overflow area is enlarged.
%
%   Example::
%     The following example counts integer bi-dimensional label
%     occurences:
%
%       K = 5 ;
%       h = zeros(1,K,'uint32') ;
%       id = zeros(2,K,'uint8');
%       next = zeros(1,K,'uint32') ;
%       X = uint8([1 1 ; 1 2 ; 2 1 ; 1 1]') ;
%       [h,id,next] = vl_ihashsum(h,id,next,K,X) ;
%
%     resulting in
%
%       h = [1 0 1 2 0]
%       id = [1    0    2    1    0
%             2    0    1    1    0]
%       next = [0 0 0 0 0]
%
%     For example, [1;2] has a count of 1 and [1;1] has a count of
%     2. NEXT is zero because there have been no collisions.
%
%   REFERENCES::
%   [1] http://en.wikipedia.org/wiki/Double_hashing
%   [2] http://www.isthe.com/chongo/tech/comp/fnv
%
%   See also: VL_IHASHFIND().

% Author: Andrea Vedaldi

% Copyright (C) 2008-12 Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
