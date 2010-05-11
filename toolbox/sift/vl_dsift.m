% VL_DSIFT  Dense SIFT
%   [FRAMES,DESCRS] = VL_DSIFT(I) extracts a dense set of SIFT
%   keypoints from image I. I must be of class SINGLE and grayscale.
%   FRAMES is a 2 x NUMKEYPOINTS, each colum storing the center (X,Y)
%   of a keypoint frame (all frames have the same scale and
%   orientation). DESCRS is a 128 x NUMKEYPOINTS matrix with one
%   descriptor per column, in the same format of VL_SIFT().
%
%   VL_DSIFT() does NOT compute a Gaussian scale space of the image
%   I. Instead, the image should be pre-smoothed at the desired scale
%   level, e.b. by using the VL_IMSMOOTH() function.
%
%   The scale of the extracted descriptors is controlled by the option
%   SIZE, i.e. the width in pixels of a spatial bin (recall that a
%   SIFT descriptor is a spatial histogram with 4 x 4 bins).
%
%   The sampling density is controlled by the option STEP, which is
%   the horizontal and vertical displacement of each feature cetner to
%   the next.
%
%   The sampled image area is controlled by the option BOUNDS,
%   defining a rectangle in which features are comptued. A descriptor
%   is included in the rectangle if all the centers of the spatial
%   bins are included. The upper-left descriptor is placed so that the
%   uppler-left spatial bin center is algined with the upper-left
%   corner of the rectangle.
%
%   By default, VL_DSIFT() computes features equivalent to
%   VL_SIFT(). However, the FAST option can be used to turn on an
%   variant of the descriptor (see VLFeat C API documentation for
%   further details) which, while not strictly equivalent, it is much
%   faster.
%
%   VL_DSIFT() accepts the following options:
%
%   Step:: 1
%     Extracts a SIFT descriptor each STEP pixels.
%
%   Size:: 3
%     A spatial bin covers SIZE pixels.
%
%   Bounds:: [whole image]
%     Specifies a rectangular area where descriptors should be
%     extracted. The format is [XMIN, YMIN, XMAX, YMAX]. If this
%     option is not specified, the entiere image is used.  The
%     bounding box is clipped to the image boundaries.
%
%   Norm::
%     If specified, adds to the FRAMES ouptut argument a third
%     row containint the descriptor norm, or engergy, before
%     contrast normalization. This information can be used to
%     suppress low contrast descriptors.
%
%   Fast::
%     If specified, use a piecewise-flat, rather than Gaussian,
%     windowing function. While this breaks exact SIFT equivalence,
%     in practice is much faster to compute.
%
%   FloatDescriptors::
%     If specified, the descriptor are returned in floating point
%     rather than integer format.
%
%   Verbose::
%     If specified, be verbose.
%
%   RELATION TO THE SIFT DETECTOR
%
%   In the standard SIFT detector/descriptor, implemented by
%   VL_SIFT(), the size of a spatial bin is related to the keypoint
%   scale by a multiplier, called magnification factor, and denoted
%   MAGNIF. Therefore, the keypoint scale corresponding to the
%   descriptors extracted by VL_DSIFT() is equal to SIZE /
%   MAGNIF. VL_DSIFT() does not use MAGNIF because, by using dense
%   sampling, it avoids detecting keypoints in the first plance.
%
%   VL_DSIFT() does not smooth the image as SIFT does. Therefore, in
%   order to obtain equivalent results, the image should be
%   pre-smoothed approriately. Recall that in SIFT, for a keypoint of
%   scale S, the image is pre-smoothed by a Gaussian of variance S.^2
%   - 1/4 (see VL_SIFT() and VLFeat C API documentation).
%
%   Example::
%     This example produces equivalent SIFT descriptors using
%     VL_DSIFT() and VL_SIFT():
%
%      binSize = 8 ;
%      magnif = 3 ;
%      Is = vl_imsmooth(I, sqrt((binSize/magnif)^2 - .25)) ;
%
%      [f, d] = vl_dsift(Is, 'size', binSize) ;
%      f(3,:) = binSize/magnif ;
%      f(4,:) = 0 ;
%      [f_, d_] = vl_sift(I, 'frames', f) ;
%
%   Remark::
%     The equivalence is never exact due to (i) boundary effects
%     and (ii) the fact that VL_SIFT() downsamples the image to save
%     computation. It is, however, usually very good.
%
%   Remark::
%     In categorization it is often useful to under-smooth the image,
%     comared to standard SIFT, in order to keep the gradients
%     sharp.
%
%   FURTHER DETAILS ON THE GEOMETRY
%
%   As mentioned, the VL_DSIFT() descriptors cover the bounding box
%   specified by BOUNDS = [XMIN YMIN XMAX YMAX]. Thus the top-left bin
%   of the top-left descriptor is placed at (XMIN, YMIN). The next
%   three bins to the right are at XMIN + SIZE, XMIN + 2*SIZE, XMIN +
%   3*SIZE. The X coordiante of the center of the first descriptor is
%   therefore at (XMIN + XMIN + 3*SIZE) / 2 = XMIN + 3/2 * SIZE.  For
%   instance, if XMIN = 1 and SIZE = 3 (default values), the X
%   coordinate of the center of the first descriptor is at 1 + 3/2 * 3
%   = 5.5. For the second descriptor immediately to its right this is
%   5.5 + STEP, and so on.
%
%   See also:: VL_HELP(), VL_SIFT().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
