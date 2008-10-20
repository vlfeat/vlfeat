% DFT Dense Feature Transform
%  [F,D] = DFT(I) calculates the Dense Histogram of Gradient
%  descriptors for image I. I must be grayscale in SINGLE format.
%
%  A VL_DHOG descriptor is equivalent to a VL_SIFT descriptor (see VL_SIFT()
%  and VLFeat API documentation). This function calculate quickly a
%  large number of such descriptors, for a dense covering of the image
%  with features of the same size and orientation.
%
%  The function returns the frames F and the descriptors D. Since all
%  frames have identical size and orientation, F has only two rows
%  (for the X and Y center coordinates). The orientation is fixed to
%  zero. The scale is related to the SIZE of the spatial bins, which
%  by default is equal to 3 pixels (see below). If NS is the number of
%  bins in each spatial direction (by default 4), then a VL_DHOG keypoint
%  covers a square patch of NS by SIZE pixels.
%
%  Remark:: The size of a VL_SIFT bin is equal to the magnification
%    factor MAGNIF (usually 3) by the scale of the VL_SIFT keypoint. For
%    instance, the scale that should be fed to VL_SIFTDESCRIPTOR() in
%    order to match the output of VL_DHOG() is equal to VL_SIFT / MAGNIF.
%
%  VL_DHOG() accepts the following options:
%
%  Step STEP [1]::
%    Extract a descriptor each STEP pixels.
%
%  Size SIZE [3]::
%    A spatial bin covers SIZE pixels.
%
%  Norm::
%    Append the frames with the normalization factor applied to each descriptor.
%    In this case, F has 3 rows and this value is the 3rd row.
%
%  Fast::
%    Use a flat rather than Gaussian window. Much faster.
%
%  Verbose::
%    Be verbose.

% AUTORIGHTS
