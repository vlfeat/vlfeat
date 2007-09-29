% OVERVIEW_VLUTIL  VisionLab MATLAB utility programs
%   This package offers a few versatile functions to manipulate
%   histograms and images.
%
%   * Use BINSUM() to compute the binned sum of an array. Use WHISTC()
%     to compute a weighted histogram. Use HISTMARG() to marginalize
%     an histogram. Use HISTNORM() to normalize an histogram.
%
%   * Use LOCALMAX() to find points of local maxima in arrays.
%
%   * Use IMSMOOTH() to quickly convolve an image by an isotropic
%     Gaussian kernel. Use IMUP() and IMDOWN() to upsample and
%     downsample images. Use IMREADGRAY() to read an image and convert
%     it to grayscale. Use IMSC() to scale an image into the [0,1]
%     interval.
%
%   * Use IMWBACKWARD() to run backward image warping. Use WAFFINE()
%     to compute an affine warp of points. Use TPS(), TPSU(), WTPS()
%     and WITPS() to compute a thin-plate-spline warp of points and
%     its inverse.
%
%   * Use RGB2DOUBLE(), RGB2XYZ() and XYZ2RGB(), XYZ2LAB() and
%     XYZ2LUV() to convert between different formats and color spaces.
%
%   * Use PLOTFRAME() to plot points, circles, oriented circles,
%     ellipses and oriented ellipses in batches.
%
%   * Use CLICK(), CLICKPOINT(), CLICKSEGMENT() to interactively
%     select image points and segments.
%
%   * Use CF() to create a copy of a figure. Use IMARRAY() and
%     IMARRAYSC() to display large arrays of images.
%
%   * Use OVERRIDE() to customize default structures by ovverriding
%     fields recursively.
%
%   * Use RODR() and IRODR() to compute Rodrigues' formula and
%     inverse with derivative. Use HAT() and IHAT() to compute the
%     hat operator.

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

help overview_vlutil ;
