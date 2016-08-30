% VL_COVDET  Covariant feature detectors and descriptors
%   VL_COVDET() implements a number of co-variant feature detectors
%   (e.g., DoG, Harris-Affine, Harris-Laplace) and corresponding
%   feature descriptors (SIFT, raw patches).
%
%   F = VL_COVDET(I) detects upright scale and translation covariant
%   features based on the Difference of Gaussian (Dog) cornerness
%   measure from image I (a grayscale image of class SINGLE). Each
%   column of F is an oriented ellipse (see VL_PLOTFRAME() for the
%   definition) even if features are upright and/or not affine
%   covariant (in which case unoriented/circular may suffice).
%
%   VL_COVDET(I, 'Method', METHOD) allows using one of the following
%   detection methods instead of the default one:
%
%   DoG:: default
%     The Difference of Gaussians is an approximate version of the
%     multiscale trace of Laplacian operator [1].
%
%   Hessian::
%     Determinant of Hessian operator [2].
%
%   HessianLaplace::
%     Determinant of Hessian for space localization, trace of
%     Laplacian for scale detection [2].
%
%   HarrisLaplace::
%     Harris cornerness measure for space localization, trace
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
%   The number of detected features is affected by the 'PeakThreshold'
%   option, which sets the minimum absolute vale of the cornerness
%   measure to accept a feature. A larger threshold causes fewer
%   features to be extracted. A good way to choose a threshold is to
%   look at the cornerness score of the features extracted from an
%   example image. This score is returned as part of the INFO
%   structure, as explained below.
%
%   In addition to the absolute value of the cornerness measure,
%   features are also filtered by the curvature of the latter. This is
%   controlled by the 'EdgeThreshold' parameter, which is the upper
%   bound on the ratio of the maximum over the minimum curvature of
%   the cornerness measure at the location of the detected
%   feature. Intuitively, a low ratio corresponds to an elongated
%   valley in the cornerness score map, which usually arises from
%   image edges. These locations are usually discarded as they tend to
%   be unstable.
%
%   Some corner detectors (e.g. HarrisLaplace) use peak in the
%   response of the multi-scale Laplace operator to select the
%   scale of the detected frames. These peaks are filtered by
%   a threshold adjustable by using the 'LaplacianPeakThreshold' option.
%
%   VL_COVDET(..., 'EstimateAffineShape', true) switches on affine
%   adaptation, an algorithm [2] that attempts to estimate the affine
%   covariant shape of each feature.
%
%   VL_COVDET(..., 'EstimateOrientation', true) switches on the
%   estimation of the orientation of the features. The algorithm looks
%   for one or more dominant orientations of the gradient in a patch
%   around the feature as in [1]. Note that more than one orientation
%   can be associated to each detected feature, creating multiple
%   versions of the same feature with different orientations. The maximum
%   number of orientations per feature can be set with MaxNumOrientations
%   option.
%
%   VL_COVDET(..., 'Frames', F) uses the user specified frames F
%   instead of running a detector. The estimation of the affine shape
%   and of the feature orientation can still be performed starting
%   from such frames. Moreover, descriptors for these frames can be
%   computed.
%
%   [F,D] = VL_COVDET(I, ...) computes the SIFT descriptors [1] for
%   the detected features. Each column of D is the descriptor of the
%   corresponding frame in F. A descriptor is a 128-dimensional vector
%   of class SINGLE. The same format of VL_SIFT() is used. SIFT
%   features are computed on normalized image patches that are
%   affected by the parameters explained next (for example, in order
%   to compute SIFT on a larger measurement region, increase the value
%   of PatchRelativeExtent).
%
%   [F,D] = VL_COVDET(I, 'descriptor', DESCRIPTOR) allows using one
%   following descriptors instead
%
%   SIFT:: default
%     The SIFT descriptor.
%
%   LIOP::
%     The Local Intensity Order Pattern descriptor. See VL_LIOP() for
%     the parameters affecting this descriptor. All LIOP parameters can
%     be used as input to VL_COVDET(), prefixed by the 'Liop' string
%     (e.g. 'LiopIntensityThrehsold').
%
%   Patch::
%     Raw patches. In this case, each column of D is a stacked square
%     image patch. This is very useful to compute alternative
%     user-defined descriptors.
%
%   The following parameters can be used to control the produced
%   descriptors:
%
%   PatchResolution:: 15 (SIFT) or 20 (LIOP, Patch)
%     The size of the patch R in pixel. Specifically, the patch is a
%     square image of side 2*R+1 pixels.
%
%   PatchRelativeExtent:: 7.5 (SIFT), 10 (LIOP), or 6 (Patch)
%     The extent E of the patch in the normalized feature frame. The
%     normalized feature frame is mapped to the feature frame F
%     detected in the image by a certain affine transformation (A,T)
%     (see VL_PLOTFRAME() for details). The patch is a square [-E,
%     E]^2 in the normalize frame, and its shape in the original image
%     is the (A,T) of it.
%
%   PatchRelativeSmoothing:: 1 (SIFT and LIOP), 1.2 (Patch)
%     The smoothing SIGMA of the patch in the normalized feature
%     frame. Conceptually, the normalized patch is computed by warping
%     the image (thought as a continuous signal) by the inverse of the
%     affine transformation (A,T) discussed above, then by smoothing
%     the wrapped image by a 2D isotropic Gaussian of standard
%     deviation SIGMA, and finally by sampling the resulting signal.
%
%   [F,D,INFO] = VL_COVDET(...) returns an additional structure INFO
%   with the following members:
%
%   info.peakScores::
%     The peak scores of the detected features.
%
%   info.edgeScores::
%     The edge scores of the detected features.
%
%   info.orientationScores::
%     The peak score of the gradient orientation histograms used to
%     assign an orientation to the detected features.
%
%   info.laplacianScaleScores::
%     The peak score of the Laplacian measure used to select
%     the scale of the detected features.
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
%   NumOctaves:: maximum possible
%     The number of scale levels sampled per octave when constructing
%     the scale spaces.
%
%   BaseScale:: 1.6
%     Gaussian Scale Space pyramid base scale. Sets up the blur which is
%     being applied to the image as sqrt(BS^2 - 0.5^2) where BS is the
%     value of the base scale and 0.5 is the initial image blur.
%
%   OctaveResolution:: 3
%     The number of scale levels sampled per octave when constructing
%     the scale spaces.
%
%   DoubleImage:: true
%     Whether to double the image before extracting features. This
%     allows to detect features at minimum smoothing level (scale) of
%     0.5 pixels rather than 1.0, resulting in many more small
%     features being detected.
%
%   MaxNumOrientations:: 4
%     Maximum number of orientations per feature when EstimateOrientation
%     is true.
%
%   AllowPaddedWarping:: true
%     Set to `false` to drop all features where measurement region gets out
%     of the input image.
%
%   Verbose::
%     If specified, it increases the verbosity level.
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
