% MSER  Maximally Stable Extremal Regions
%   R=MSER(I) computes the Maximally Stable Extremal Regions (MSER)
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
%   [R,F]=MSER(...) also returns ellipsoids F fitted to the regions.
%   Each column of the matrix ELL describes an ellipsoid; F(1:D,i) is
%   the center of the elliposid and F(D:end,i) the independent elments
%   of the co-variance matrix of the ellipsoid.
%   
%   Ellipsoids are computed according to the same reference frame of I
%   seen as a matrix. This means that coordinates start at (1,1,...,1)
%   and that the first coordinate spans the first dimension of I.
%
%   Notice that for 2-D images usually the opposite convention is used
%   (i.e. the first coordinate is the x-axis, which corresponds to the
%   column index). Thus, if the function PLOTFRAME() is used to plot
%   the ellipses, the frames F should be `transoposed' as in F = F([2
%   1 5 4 3],:).
%
%   MSER(I,'Option'[,Value]...) accepts the following options
%
%   Delta::
%       Set the DELTA parameter of the MSER algorithm. Roughly
%       speaking, the stability of a region is the relative variation
%       of the region area when the intensity is changed of +/-
%       Delta/2.
%
%   Epsilon::
%       Set the EPSILON parameter of the MSER algorithm. When the
%       relative area variation of two nested regions is below
%       this treshold, then only the most stable one is selected.
%
%   MaxArea::
%       Set the maximum area (volume) of the regions relative to
%       the image domain area (volume).
%
%   MinArea::
%       Set the minimum area (volume) of the regions relative to 
%       the image domain area (volume).
%
%   MaxVariation::
%       Set the maximum variation (absolute stability score) of the
%       regions.
%
%   Dups::
%       Activate dupliate regions removal.
%
%   NoDups::
%       Deactivate duplcate regions removal.
%
%   Verbose::
%       Be verbose.
%
%   REFERENCES
%   [1] J. Matas, O. Chum, M. Urban, and T. Pajdla, "Robust wide
%       baseline stereo from maximally stable extremal regions," in
%       Proc. BMVC, 2002.

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
