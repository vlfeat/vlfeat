% VL_UBCMATCH  Match SIFT features
%   MATCHES = VL_UBCMATCH(DESCR1, DESCR2) matches the two sets of SIFT
%   descriptors DESCR1 and DESCR2.
%   
%   [MATCHES,SCORES] = VL_UBCMATCH(DESCR1, DESCR2) retuns the matches and
%   also the squared Euclidean distance between the matches.
%
%   The function uses the algorithm suggested by D. Lowe [1] to reject
%   matches that are too ambiguous.
%
%   VL_UBCMATCH(DESCR1, DESCR2, THRESH) uses the specified threshold
%   THRESH. A descriptor D1 is matched to a descriptor D2 only if the
%   distance d(D1,D2) multiplied by THRESH is not greater than the
%   distance of D1 to all other descriptors. The default value of
%   THRESH is 1.5.
%   
%   The storage class of the descriptors can be either DOUBLE, FLOAT,
%   INT8 or UINT8. Usually integer classes are faster.
%   
%   REFERENCES
%
%   [1] D. G. Lowe, Distinctive image features from scale-invariant
%       keypoints. IJCV, vol. 2, no. 60, pp. 91-110, 2004.
%
%   See also VL_HELP(), VL_SIFT().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
