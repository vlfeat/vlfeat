% VL_DHOG Dense SIFT
%   [F,D] = VL_DSIFT(I) calculates the Dense Histogram of Gradients
%   (DSIFT) descriptors for the image I. I must be grayscale in SINGLE
%   format.
%
%   In this implementation, a DSIFT descriptor is equivalent to a SIFT
%   descriptor (see VL_SIFT()). This function calculates quickly a
%   large number of such descriptors, for a dense covering of the image
%   with features of the same size and orientation.
%
%   The function returns the frames F and the descriptors D. Since all
%   frames have identical size and orientation, F has only two rows
%   (for the X and Y center coordinates). The orientation is fixed to
%   zero. The scale is related to the SIZE of the spatial bins, which
%   by default is equal to 3 pixels (see below). If NS is the number of
%   bins in each spatial direction (by default 4), then a DSIFT keypoint
%   covers a square patch of NS by SIZE pixels.
%
%   Remark:: The size of a SIFT bin is equal to the magnification
%     factor MAGNIF (usually 3) by the scale of the SIFT
%     keypoint. This means that the scale of the SIFT keypoints
%     corresponding to the DSIFT descriptors is SIZE / MAGNIF.
%
%   Remark:: Although related, DSIFT is not the same as the HOG
%     descriptor used in [1]. This descriptor is equivalent to
%     SIFT instead.
%
%   VL_DSIFT() accepts the following options:
%
%   Step STEP [1]::
%     Extract a descriptor each STEP pixels.
%
%   Size SIZE [3]::
%     A spatial bin covers SIZE pixels.
%
%   Norm::
%     Append the frames with the normalization factor applied to each
%     descriptor. In this case, F has 3 rows and this value is the 3rd
%     row. This information can be used to suppress descriptors
%     with low contrast.
%
%   Fast::
%     Use a flat rather than Gaussian window. Much faster.
%
%   Verbose::
%     Be verbose.
%
%  References
%
%  [1] N. Dalal and B. Triggs. Histograms of oriented gradients for
%      human detection. In Proc. CVPR, 2005.
%
%  See also:: VL_HELP(), VL_SIFT().

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
