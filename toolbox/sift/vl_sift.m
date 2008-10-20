% VL_SIFT  Scale-invariant feature transform
%   F = VL_SIFT(I) computes the VL_SIFT frames (keypoints) F of the image
%   I. I is a gray-scale image in single precision. Each column of F
%   is a feature frame and has the format [X;Y;S;TH], where X,Y is the
%   (fractional) center of the frame, S is the scale and TH is the
%   orientation (in radians).
%
%   [F,D] = VL_SIFT(I) computes in addition the VL_SIFT descriptors D. Each
%   column of D is the descriptor of the corresponding frame in F. A
%   descriptor is a 128-dimensional vector of storage class UINT8.
%
%   Options:
%
%   Octaves::
%       Set the number of octave of the DoG scale space.
%
%   Levels::
%       Set the number of levels per octave of the DoG scale space.
%
%   FirstOctave::
%       Set the index of the first octave of the DoG scale space.
%
%   PeakThresh::
%       Set the peak selection threshold.
%
%   EdgeThresh::
%       Set the non-edge selection threshold.
%
%   NormThresh::
%       Set the minimum l2-norm of the descriptor before
%       normalization. Descriptors below the threshold are set to
%       zero.
%
%   Magnif::
%       Set the descriptor magnification factor. The scale of the
%       keypoint is multiplied by this factor to obtain the width (in
%       pixels) of the spatial bins. For instance, since by default
%       there are 4 spatial bins along each spatial direction, the
%       ``diameter'' of the descriptor is approximatively 4 * MAGNIF.
%
%   Frames::
%       Set the frames to use (bypass the detector). If frames are not
%       passed in order of increasing scale, they are re-orderded.
%
%   Orientations::
%       Compute the orietantions of the frames overriding the 
%       orientation specified by the 'Frames' option.
%
%   Verbose::
%       Be verbose (may be repeated to increase the verbosity level).
%
%  See also HELP_VLFEAT, VL_UBCMATCH().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
