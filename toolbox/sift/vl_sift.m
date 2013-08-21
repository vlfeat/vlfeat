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
%   VL_SIFT() accepts the following options:
%
%   Octaves:: maximum possible
%     Set the number of octave of the DoG scale space.
%
%   Levels:: 3
%     Set the number of levels per octave of the DoG scale space.
%
%   FirstOctave:: 0
%     Set the index of the first octave of the DoG scale space.
%
%   PeakThresh:: 0
%     Set the peak selection threshold.
%
%   EdgeThresh:: 10
%     Set the non-edge selection threshold.
%
%   NormThresh:: -inf
%     Set the minimum l2-norm of the descriptors before
%     normalization. Descriptors below the threshold are set to zero.
%
%   Magnif:: 3
%     Set the descriptor magnification factor. The scale of the
%     keypoint is multiplied by this factor to obtain the width (in
%     pixels) of the spatial bins. For instance, if there are there
%     are 4 spatial bins along each spatial direction, the
%     ``side'' of the descriptor is approximatively 4 * MAGNIF.
%
%   WindowSize:: 2
%     Set the variance of the Gaussian window that determines the
%     descriptor support. It is expressend in units of spatial
%     bins.
%
%   Frames::
%     If specified, set the frames to use (bypass the detector). If
%     frames are not passed in order of increasing scale, they are
%     re-orderded.
%
%   Orientations::
%     If specified, compute the orientations of the frames overriding
%     the orientation specified by the 'Frames' option.
%
%   Verbose::
%     If specfified, be verbose (may be repeated to increase the
%     verbosity level).
%
%   REFERENCES::
%     [1] D. G. Lowe, Distinctive image features from scale-invariant
%     keypoints. IJCV, vol. 2, no. 60, pp. 91-110, 2004.
%
%   See also: <a href="matlab:vl_help('sift')">SIFT</a>
%   VL_UBCMATCH(), VL_DSIFT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
