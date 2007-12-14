% SIFTMATCH  Match SIFT features
%   MATCHES=SIFTMATCH(DESCR1, DESCR2) matches the two sets of SIFT
%   descriptors DESCR1 and DESCR2.
%
%   The function uses the same algorithm suggested by D. Lowe [1] to
%   reject matches that are too ambiguous.
%
%   SIFTMATCH(DESCR1, DESCR2, THRESH) uses [1] with the specified
%   threshold THRESH. A descriptor D1 is matched to a descriptor D2
%   only if the distance d(D1,D2) multiplied by THRESH is not greather
%   than the distance of D1 to all other descriptors. The default
%   value of THRESH is 1.5.
%   
%   The storage class of the descriptors can be either DOUBLE, FLOAT,
%   INT8 or UINT8. Usually interger classes are faster.
%   
%   [1] D. G. Lowe, 
%       `Distinctive image features from scale-invariant keypoints,'
%       IJCV, vol. 2, no. 60, pp. 91–110, 2004.
%
%   See also SIFT().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
