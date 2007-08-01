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
% Copyright (C) 2006 Andrea Vedaldi
%       
% This file is part of VLUtil.
% 
% VLUtil is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software Foundation,
% Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

help overview_vlutil ;
