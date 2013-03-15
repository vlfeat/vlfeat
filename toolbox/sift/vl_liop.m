% VL_LIOP Local Intensity Order Pattern descriptor
%   D = VL_LIOP(I) compute the Liop descriptor of an image I.
%   I is a square patch of an odd side length. Pixels belonging
%   to the area of incircle of the patch will be used for
%   computation. D is a column vector containing the liop
%   descriptor of I. Implementation according to [1].
%
%   Use VL_COVDET() for computation of Liop descriptor from features frames.
%
%   VL_LIOP() accepts the following options:
%
%   Neighbours:: [4]
%     Set the number of neighbours used for computation of permutation index.
%
%   Bins:: [6]
%     Set the number of bins to which the points will be quintized according
%     to their intensity.
%
%   (the final descriptor will be of length factorial(neighbours)*bins)
%
%   Radius:: [5]
%     Set the distance between a point and it's neighbours.
%
%   WeightThreshold:: [see below]
%     Set the weight threshold (i.e. how big must be the difference between
%     intensities of all neighbouring points so that they influence descriptor
%     value significantly).
%     If not specified, the threshold is set according to the intensity values
%     range:
%           1) all values < 1    -> [0.02]
%           2) all values < 255  -> [2]
%           3) other             -> [maxIntensityValue * 0.02]
%
%   Verbose::
%     If specified, be verbose
%
%   REFERENCES::
%     [1] Z. Wang, B. Fan, F. Wu. Local Intensity Order Pattern for feature
%         description. In ICCV, 2011
%




