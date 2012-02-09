% VL_HELP  VLFeat MATLAB Tooblox
%   The VLFeat MATLAB toolbox provides implementations of common
%   computer vision algorithms (SIFT, MSER, AIB, KMEANS, ...) and
%   other useful MATLAB functions, some of which are listed next:
%
%   * VL_SIFT() computes the Scale-Invariant Feature Transform.  SIFT
%     bundels the most widely used feature detector and
%     descriptor. This version is compatbile with Lowe's original
%     implementation.
%
%   * VL_MSER() computes the Maximally-Stable Extremal Regions of an
%     image. It detects features corresponding to stable image level
%     sets.
%
%   * VL_IKMEANS() and VL_HIKMEANS() are basic implementations of K-means
%     and hierarchical IKM optimized to work on integer data
%     types. This is useful to quantize large collection of features,
%     for instance to create visual dictionaries.
%
%   * VL_AIB() uses the AIB algorithm to discriminatively compress a
%     discrete random variable. An application is the compresssion
%     of a visual dictionary for categorization.
%
%   * VL_IMARRAY() and VL_IMARRAYSC() tile image stacks, VL_IMSC()
%     scales the range of an image, VL_IMSMOOTH() convolves an image
%     by a number of different kernels, VL_IMWBACKWARD() warps an
%     image by backward mapping, VL_WAFFINE(), VL_WTPS(), VL_WITPS()
%     compute various kind of image warps, VL_IMWHITEN() whitens an
%     image, VL_XYZ2LAB(), VL_XYZ2LUV(), VL_XYZ2RGB(), VL_RGB2XYZ()
%     perform basic color space conversions.
%
%   * VL_CF() to copies a figure, VL_TIGHTSUBPLOT() provides a
%     "borderless" version of SUBPLOT(), VL_CLICK() and
%     VL_CLICKPOINT() interactively select image points,
%     VL_PRINTSIZE() scales a figure printing size to a fraction of
%     the default paper size, VL_PLOTFRAME() to plot feature frames
%     (points, disks, oriented disks, ellipses, oriented ellpises).
%
%   * VL_BINSUM() computeds binned summations (and compute
%     histograms), VL_TWISTER() controls and runs VLFeat internal
%     random number generator (useful to reproduce results from
%     randomized algorithms), VL_WHISTC() computes weighed histograms,
%     VL_GRAD() computes the gradient of a 2-D function.
%
%   See also: http://www.vlfeat.org.

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

help vl_help.m
