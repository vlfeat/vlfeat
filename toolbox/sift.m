% SIFT  Scale-invariant feature transform
%   F = SIFT(I) where computes the SIFT frames (keypoints) F of the
%   image I. I is a gray-scale image in single precision. Each column
%   of F is a feature frame and has the format [X;Y;S;TH], where X,Y
%   is the (fractional) center of the frame, S is the scale and TH is
%   the orientation (in radians).
%
%   [F,D] = SIFT(I) computes in addition the SIFT descriptors D. Each
%   column of D is the descriptor of the corresponding frame in F. A
%   descriptor is a 128-dimensional vector of storage class UINT8.
%
%   MSER(I,'Option'[,Value]...) accepts the following options
%
%   Octaves
%       Set the number of octave of the DoG scale space.
%
%   Levels
%       Set the number of levels per octave of the DoG scale space.
%
%   FirstOctave
%       Set the index of the first octave of the DoG scale space.
%
%   PeakTresh
%       Set the peak selection treshold.
%
%   EdgeTresh
%       Set the non-edge selection treshold.
%
%   Frames
%       Set the frames to use (bypass the detector).
%
%   Orientations
%       Force the computation of the oritentations of the frames
%       even if the option 'Frames' is being used.
%
%   Verbose
%       Be verbose (may be repeated).

% AUTORIGHTS
