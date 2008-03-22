% HELP_VLFEAT  VLFeat MATLAB Tooblox
%   The VLFeat MATLAB toolbox provides implementation of common
%   computer vision algorithms (SIFT, MSER, AIB, KMEANS, ...) and
%   other useful MATLAB functions. VLFeat inclues numerous functions,
%   some of which are higlighted next.
%
%   * SIFT() computes the Scale-Invariant Feature Transform of an
%     image. SIFT bundels the most widely used feature detector and
%     descriptor. This version is compatbile with Lowe's original
%     implementation.
%
%   * MSER() computes the Maximally-Stable Extremal Regions of an
%     image. It detects features corresponding to stable image level
%     sets.
%
%   * IKMEANS() and HIKMEANS() are basic implementations of K-means
%     and hierarchical IKM optimized to work on integer data
%     types. This is useful to quantize large collection of features,
%     for instance to create visual dictionaries.
% 
%   * AIB() compresses an informative random variable based on the AIB
%     algorithm. This can be used, for instance, to compress a visual
%     dictionary while preserving its discriminative power as much as
%     possible.
%
%   * VLfeat bundels functions to operate on images. You can use
%     IMARRAY() and IMARRAYSC() to tile images, IMSC() to scale the
%     range of an image, IMSMOOTH() to convolve an image by a Gaussian
%     kernel, IMWBACKWARD() to warp an image by bacward mapping,
%     WAFFINE(), WTPS(), WITPS() to compute various kind of image
%     warps, IMWHITEN() to whiten an image, XYZ2LAB(), XYZ2LUV(),
%     XYZ2RGB(), RGB2XYZ() to perform basic color space conversions.
%
%   * VLFeat bundles a few functions that help operating with
%     plots. For instance, you can use CF() to copy a figure,
%     TIGHTSUBPLOT() to get a borderless version of SUBPLOT(), CLICK()
%     and CLICKPOINT() to interactively select image points,
%     PRINTSIZE() to scale a figure printing size to a fraction of the
%     default paper size, PLOTFRAME() to plot feature frames (points,
%     disks, oriented disks, ellipses, oriented ellpises).
%
%   * VLFeat bundles other useful functions. You can use BINSUM() to
%     computed binned summations (and compute histograms), TWISTER()
%     to control and use the VLFeat random number generator (useful to
%     reproduce results from randomized algorithms), WHISTC() to
%     compute weighed histograms, GRAD() To compute the gradient of a
%     2-D function.
%
%   See also:: http://vision.ucla.edu/vlfeat

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

help help_vlfeat.m
