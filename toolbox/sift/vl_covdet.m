% VL_COVDET  Covariant feature detectors and descriptors
%   VL_COVDET() detects covariant features such as SIFT [1], Hessian,
%   Harris keypoints, their scale and adapted variants [2], and
%   computes the corresponding SIFT descriptors.
%
%   F = VL_COVDET(I) computes the SIFT features for image I [1]. F is
%   a matirx storing the frame of one SIFT keypoint per column in the
%   [X;Y;S;TH] format, where X, Y is the frame center, S its scale,
%   and TH its orientation.
%
%   VL_COVDET(I, 'Method', M) selects a feature extraction method
%   from:
%
%   DoG:: Difference of Gaussian operator
%     This is the default method and the method used by SIFT. Features
%     are detected as local maxima (in space and scale) of the
%     Lapalacian operator response.
%
%   Hessian:: Hessian operator
%     Features are detected as local maxima (in space and scale) of
%     the Hessian operator response.
%
%   F = VL_COVDET(I, 'AffineAdaptation', true) turns on the affine
%   adapation of the features based on the second moment
%   matrix. VL_COVDET(I, 'Orientation', false) turns off the detection
%   of the feature orientation (so that features are taken to be
%   upright). Based on different combinations of these options, the
%   format of the feature frames F changes as follows:
%
%     AffineAdaptation  Orientation Frame Type        Frame format
%     false*            false       disc              [X;Y;S]
%     false*            true*       oriented disc     [X;Y;S;TH]
%     true              false       ellipse           [X;Y;E11;E12;E22]
%     true              true*       oriented ellipse  [X;Y;A11;A12;A21;A22]
%
%   where * denotes the default value and the parameters are given by:
%
%   [X, Y]:: coordinate center
%   S:: scale
%   TH:: orientation (in radians)
%   [E11,E12,E22]:: ellpitical shape
%      This is in the format , which is the
%      inverse...
%   [A11,A12;A21,A22]::
%      Matirx A mapping
%
%   [F,D] = VL_COVDET(I, ...) computes the SIFT descriptors [1] for
%   the detected features. Each column of D is the descriptor of the
%   corresponding frame in F. A descriptor is a 128-dimensional vector
%   of class UINT8 or DOUBLE when 'FloatDescriptors' option is
%   specified.
%
%   [F,D,GSS,RESP] = VL_COVDET(I, ...) returns the Gaussian scale
%   space GSS of the image I and the response scale space RESP of the
%   operator used to detect features in space and scale (its nature
%   depends on the 'Method' option). For the format of GSS and RESP
%   see VL_PLOTSS().
%
%   [F,D] = VL_COVDET(I,...,'Frames',FI) takes the frames FI as input,
%   converts them to frames of a different type F asspecified by the
%   parameters 'AffineAdaptation' and 'Orientation' as described above
%   before.
%
%   VL_COVDET(...,'OptionName',value) accepts the following options:
%
%   Octaves:: [maximum possible]
%     Set the number of octave of the DoG scale space.
%
%   Levels:: [3]
%     Set the number of levels per octave of the DoG scale space.
%
%   FirstOctave:: [0]
%     Set the index of the first octave of the DoG scale space. If
%     FirstOctave < 0 the image I is upsampled and for FirstOctave > 0
%     image is downsampled.
%
%   PeakThresh:: [0 for DoG, 28.5 for Hessian]
%     Set the peak selection threshold.
%
%   EdgeThresh:: [10]
%     Set the non-edge selection threshold.
%
%   NormThresh:: [-inf]
%     Set the minimum l2-norm of the descriptors before
%     normalization. Descriptors below the threshold are set to zero.
%
%   Magnif:: [3]
%     Set the descriptor magnification factor. The scale of the
%     keypoint is multiplied by this factor to obtain the
%     neighbourhood used for descriptor calculation.  In the case of
%     isotropic frames (discs) it defines the size of the spatial
%     bins. For instance, if there are 4 spatial bins along each
%     spatial direction, the ``side'' of the descriptor is
%     approximatively 4 * MAGNIF. In the case of anisotropic frames
%     (ellipses) it defines the size of keypoint neighbourhood which
%     is normalised into a square patch used fort descriptor
%     computation.
%
%   WindowSize:: [2]
%     Set the variance of the Gaussian window that determines the
%     descriptor support. It is expressend in units of spatial
%     bins. Used only for isotropic frames (discs).
%
%   AffWinSize:: [19]
%     Size of the window used for Second Moment Matrix (SMM)
%     calculation. SMM is used as an estimation of the affine
%     shape. Used only for anisotropic frames (ellipses).
%
%   AffMaxIter:: [16]
%     Maximum number of iterations for the affine shape estimation.
%     If the convergence criterion is not fulfilled in these steps,
%     keypoint is rejected. Used only for anisotropic frames (ellipses).
%
%   AffConvThr:: [0.05]
%     Convergence criterion for affine shape estimation.
%     Affine shape is accepted when:
%
%           \lambda_min(SMM)
%       1 - ----------------  < AffConvThr
%           \lambda_max(SMM)
%
%     Used only for anisotropic frames (ellipses).
%
%   SIFTPatchSize:: [41]
%     Size of the patch used for SIFT descriptor calculation. Used only
%     for anisotropic frames (ellipses).
%
%   Frames:: [not specified]
%     If specified, set the frames to use (bypass the detector).
%
%   FloatDescriptors::
%     If specified, descriptors are returned as float numbers.
%
%   Verbose::
%     If specfified, be verbose (may be repeated to increase the
%     verbosity level).
%
%   REFERENCES::
%     [1] D. G. Lowe, Distinctive image features from scale-invariant
%     keypoints. IJCV, vol. 2, no. 60, pp. 91-110, 2004.
%
%     [2] K. Mikolajcyk and C. Schmid, An affine invariant interest
%     point detector. ICCV, vol. 2350, pp. 128-142, 2002.
%
%   See also: VL_UBCMATCH(), VL_DSIFT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
