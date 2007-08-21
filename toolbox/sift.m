% SIFT  Scale-invariant feature transform
%
%   F = SIFT(I) where I is a gray-scale image in single precision
%   computes the SIFT frames (keypoints) F of the image. A frame is a
%   column of the matrix F and has the format [X;Y;S;TH], where X,Y is
%   the (fractional) center of the frame, S is the scale and TH is the
%   orientation (in radians).
%
%   [F,D] = SIFT(I) computes in addition the SIFT descriptors D. Each
%   column of D is the descriptor of the corresponding frame in F. A
%   descriptor is a 128-dimensional vector of storage class UINT8.
%
%   SIFT(I,opts...) accepts the following options:
%
%   Octaves
%   Levels
%   FirstOctave
%   PeakTresh
%   EdgeTresh
%   Orientations
%   Verbose



