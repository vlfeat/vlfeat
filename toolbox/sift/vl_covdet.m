% VL_COVDET  Covariant feature detectors and descriptors
%   VL_COVDET() implements a number of co-variant feature detectors
%   (e.g., DoG, Harris-Affine, Harris-Laplace) and allow to compute
%   corresponding feature descriptors (SIFT, raw patches).
%
%   F = VL_COVDET(I) detects upright scale and translation covariant
%   features based on the Difference of Gaussian (Dog) cornerness
%   measure from image I (a grayscale image of class SINGLE). F is in
%   the format of oriented ellipse feature frames (see VL_PLOTFRAME()
%   for the definition) even if features are only scale-invariant
%   (discs or oriented discs).
%
%   VL_COVDET(I, 'Method', METHOD) allows using one of the
%   following methods instead:
%
%   DoG::
%     The Difference of Gaussians is an approximate version of the
%     multiscale trace of Laplacian operator [1].
%
%   Hessian::
%     Determinant Hessian operator [2].
%
%   HessianLaplace::
%     Determinant of Hessian for space localisation, trace of
%     Laplacian for scale detection [2].
%
%   HarrisLaplace::
%     Harris cornerness measure for space localisation, trace
%     of Laplacian for scale detection [2].
%
%   MultiscaleHessian::
%     Same as HessianLaplace, but Laplacian scale detection is not
%     performend (features are simply detected at multiple scales) [2].
%
%   MultiscaleHarris::
%     Same as HarrisLaplace, but Laplacian scale detection is not
%     performend (features are simply detected at multiple scales) [2].
%
%   The number of detected features is affected by the
%   'PeakThreshold', which sets the minimum absolute vale of the
%   cornerness measure to accept a feature. A larger threshold select
%   fewer features. To adjust the threshold, the score of the detected
%   features can be obtained in the INFO structure (see later).
%
%   Features can also be filtered by setting the 'EdgeThreshold'
%   parameter, which sets an upper bound on the ratio of the maxium
%   over the minium curvature of the cornerness measure at the
%   detected location. The idea is that unbalanced curvatures
%   correspond to features detected along image edges, and should
%   therefore be discarded as spatially unstable.
%
%   VL_COVDET(..., 'EstimateAffineShape', true) switches on affine
%   adaptation, which attempts to estimate the affine co-variant shape
%   of each feature based on the algorihtm of [2].
%
%   VL_COVDET(..., 'EstimateOrientation', true) switches on the
%   estimation of the orientation of features (which are therefore not
%   upright anymore) []. Note that more than one orientation can be
%   associated to each feature, creating copies of them.
%
%   VL_COVDET(..., 'Frames', F) allows to specify user defined frames
%   F. This skips detection, but estimating the affine shape or the
%   orietnations can still be applied. Moreover, descriptors for these
%   frames can be computed.
%
%   [F,D] = VL_COVDET(I, ...) computes the SIFT descriptors [1] for
%   the detected features. Each column of D is the descriptor of the
%   corresponding frame in F. A descriptor is a 128-dimensional vector
%   of class SINGLE. The same format of VL_SIFT() is used. SIFT
%   features are computed on normalised image patches that are
%   affected by the parameters explained next (for example, to comptue
%   SIFT on a larger measurement reagion, increase the value of
%   PatchRelativeExtent.
%
%   [F,D] = VL_COVDET(I, 'descriptor', DESCRIPTOR) allows using one
%   following descriptors instead
%
%   SIFT::
%     The default SIFT descriptor.
%
%   LIOP::
%     The Local Intensity Order Pattern descriptor. See VL_LIOP() for
%     parameter definitions. All listed parameters can be used as
%     input to VL_COVDET(), prefixed by the 'Liop' string (e.g.
%     'LiopIntensityThrehsold').
%
%   PATCH::
%     Raw patches. In this case, each column of D is a stacked square
%     image patch. This is very useful to compute alternative
%     descriptors.
%
%   The following parameters can be used to control the produced
%   descriptors:
%
%   PatchResolution:: for SIFT descriptor 15, LIOP 20, PATCH 20
%     The size of the patch R in pixel. Specifically, the patch is a
%     square of side 2*R+1 pixels.
%
%   PatchRelativeExtent:: for SIFT descriptor 7.5, LIIP 10, PATCH 6
%     The extent E of the patch in the feature frame. A feature F
%     define a mapping from the feature reference frame to the image
%     reference frame as an affine transformation A,T (see
%     VL_PLOTFRAME()). The patch is a square [-E, E]^2 in this frame
%     (transform this square by A,T to find the extent in the image).
%
%   PatchRelativeSmoothing:: for SIFT descriptor 1, LIOP 1, PATCH 1.2
%     The smoothing SIGMA of the patch in the patch frame. The
%     computed patch can be thought as being obtained by first
%     warping the image (as a continous signal) by A,T, then
%     smoothing the results by SIGMA, and then sampling.
%
%   [F,D,INFO] = VL_COVDET(...) returns an additiona structure INFO
%   with the following members:
%
%   info.peakScores::
%     The peak scores of the detected features.
%
%   info.edgeScores::
%     The edge scores of the detected features.
%
%   info.gss::
%     The Gaussian scale space (see VL_PLOTSS()).
%
%   info.css::
%     The cornerness measure scale space (see VL_PLOTSS()).
%
%   In addition to the ones discussed so far, the function supports
%   the following options:
%
%   OctaveResolution:: 3
%     The number of scale levels sampled per octave when constructing
%     the scale spaces.
%
%   DoubleImage:: true
%     Whether to double the image before extracting features. This
%     allows to detect features at a smoothing level of 0.5 and up
%     rathern than 1.0 and up, resulting in many more small
%     features being detected.
%
%   Verbose::
%     If specified, it incerases the verbosity level.
%
%   REFERENCES::
%   [1] D. G. Lowe, Distinctive image features from scale-invariant
%   keypoints. IJCV, vol. 2, no. 60, pp. 91-110, 2004.
%
%   [2] K. Mikolajcyk and C. Schmid, An affine invariant interest
%   point detector. ICCV, vol. 2350, pp. 128-142, 2002.
%
%   See also: VL_SIFT(), VL_LIOP(), VL_PLOTFRAME(), VL_PLOTSS(), VL_HELP().

% Copyright (C) 2007-12 Karel Lenc, Andrea Vedaldi, and Michal Perdoch
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
