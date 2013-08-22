% VL_HOG Compute HOG features
%   HOG = VL_HOG(IM, CELLSIZE) computes the HOG features for image IM
%   and the specified CELLSIZE. IM can be either grayscale or colour
%   in SINGLE storage class. HOG is an array of cells: its number
%   of columns is approximately the number of columns of IM divided
%   by CELLSIZE and the same for the number of rows. The third
%   dimension spans the feature compoents.
%
%   PERM = VL_HOG('permutation') returns the left-right permutation
%   to apply to each HOG cell to flip it.
%
%   IMAGE = VL_HOG('render', HOG) returns an IMAGE containing an
%   iconic representation of the array of cells HOG.
%
%   Options:
%
%   Variant:: 'UoCTTI'
%     Choose a HOG variant: 'UoCTTI' or 'DalalTriggs'.
%
%   NumOrientations:: 9
%     Choose a number of undirected orientations in the orientation
%     histograms. The angle [0,pi) is divided in to NumOrientation
%     equal parts.
%
%   DirectedPolarField::
%     By specifying this flag the image IM is interpreted as samples
%     from a 2D vector field specified by their argument IM(:,:,2) and
%     modulus IM(:,:,1).
%
%   UndirectedPolarField::
%     Same as above, but wraps angles in [0,pi).
%
%   BilinearOrientations::
%     This flags activates the use of bilinear interpolation to assign
%     orientations to bins. This produces a smoother feature, but is
%     not some other implementations (e.g. UoCTTI).
%
%   Example:: computing and visualizing HOG features
%     hog = vl_hog(im2single(im)) ; % compute HOG features
%
%   See also: <a href="matlab:vl_help('hog')">HOG fundamentals</a>,
%   VL_HELP().

% Author: Andrea Vedaldi

% Copyright (C) 2012-13 Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
