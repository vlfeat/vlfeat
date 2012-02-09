% VL_MSER  Maximally Stable Extremal Regions
%   R=VL_MSER(I) computes the Maximally Stable Extremal Regions (MSER)
%   [1] of image I with stability threshold DELTA. I is any array of
%   class UINT8. R is a vector of region seeds.
%
%   A (maximally stable) extremal region is just a connected component
%   of one of the level sets of the image I.  An extremal region can
%   be recovered from a seed X as the connected component of the level
%   set {Y: I(Y) <= I(X)} which contains the pixel o index X.
%
%   The function supports images of arbitrary dimension D.
%
%   [R,F]=VL_MSER(...) also returns ellipsoids F fitted to the regions.
%   Each column of F describes an ellipsoid; F(1:D,i) is the center of
%   the elliposid and F(D:end,i) are the independent elements of the
%   co-variance matrix of the ellipsoid.
%
%   Ellipsoids are computed according to the same reference frame of I
%   seen as a matrix. This means that the first coordinate spans the
%   first dimension of I.
%
%   Notice that for 2-D images usually the opposite convention is used
%   (i.e. the first coordinate is the x-axis, which corresponds to the
%   column index). Thus, if the function VL_PLOTFRAME() is used to plot
%   the ellipses, the frames F should be `transposed' as in F = F([2
%   1 5 4 3],:). VL_ERTR() exists for this purpose.
%
%   VL_MSER(I,'Option'[,Value]...) accepts the following options
%
%   Delta:: [5]
%       Set the DELTA parameter of the VL_MSER algorithm. Roughly
%       speaking, the stability of a region is the relative variation
%       of the region area when the intensity is changed of +/-
%       Delta/2.
%
%   MaxArea:: [0.75]
%       Set the maximum area (volume) of the regions relative to
%       the image domain area (volume).
%
%   MinArea:: [3 / numPixels]
%       Set the minimum area (volume) of the regions relative to
%       the image domain area (volume).
%
%   MaxVariation:: [0.25]
%       Set the maximum variation (absolute stability score) of the
%       regions.
%
%   MinDiversity:: [0.2]
%       Set the minimum diversity of the region. When the relative
%       area variation of two nested regions is below this threshold,
%       then only the most stable one is selected.
%
%   BrightOnDark:: [1]
%       Detect bright-on-dark MSERs. This corresponds to MSERs of the
%       inverted image.
%
%   DarkOnBright:: [1]
%       Detect dark-on-bright MSERs. This corresponds to MSERs of the
%       original image.
%
%   Verbose::
%       Be verbose.
%
%   REFERENCES::
%     [1] J. Matas, O. Chum, M. Urban, and T. Pajdla, "Robust wide
%     baseline stereo from maximally stable extremal regions," in
%     Proc. BMVC, 2002.
%
%   See also: VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
