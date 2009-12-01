% VL_SIFT  Scale-Invariant Feature Transform
%   F = VL_SIFT(I) computes the SIFT frames [1] (keypoints) F of the
%   image I. I is a gray-scale image in single precision. Each column
%   of F is a feature frame and has the format [X;Y;S;TH], where X,Y
%   is the (fractional) center of the frame, S is the scale and TH is
%   the orientation (in radians).
%
%   [F,D] = VL_SIFT(I) computes the SIFT descriptors [1] as well. Each
%   column of D is the descriptor of the corresponding frame in F. A
%   descriptor is a 128-dimensional vector of class UINT8.
%
%   Options:
%
%   Octaves::
%     Set the number of octave of the DoG scale space.
%
%   Levels::
%     Set the number of levels per octave of the DoG scale space.
%     The default value is 3.
%
%   FirstOctave::
%     Set the index of the first octave of the DoG scale space.
%     The default value is 0.
%
%   PeakThresh::
%     Set the peak selection threshold.
%     The default value is 0.
%
%   EdgeThresh::
%     Set the non-edge selection threshold.
%     The default value is 10.
%
%   NormThresh::
%     Set the minimum l2-norm of the descriptor before
%     normalization. Descriptors below the threshold are set to
%     zero.
%
%   Magnif::
%     Set the descriptor magnification factor. The scale of the
%     keypoint is multiplied by this factor to obtain the width (in
%     pixels) of the spatial bins. For instance, if there are there
%     are 4 spatial bins along each spatial direction, the
%     ``diameter'' of the descriptor is approximatively 4 * MAGNIF.
%     The default value is 3.
%
%   WindowSize::
%     Set the variance of the Gaussian window that determines the
%     descriptor support. It is expressend in units of spatial
%     bins and the default value is 2.
%
%   Frames::
%     Set the frames to use (bypass the detector). If frames are not
%     passed in order of increasing scale, they are re-orderded.
%
%   Orientations::
%     Compute the orietantions of the frames overriding the 
%     orientation specified by the 'Frames' option.
%
%   Verbose::
%     Be verbose (may be repeated to increase the verbosity level).
%
%  REFERENCES
% 
%  [1] D. G. Lowe, Distinctive image features from scale-invariant
%       keypoints. IJCV, vol. 2, no. 60, pp. 91-110, 2004.
%
%  See also VL_HELP(), VL_UBCMATCH().

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
