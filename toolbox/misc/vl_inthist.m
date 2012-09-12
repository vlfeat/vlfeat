% VL_INTHIST  Calculate Integral Histogram
%   INTHIST = VL_INTHIST(LABELS) computes the integral histogram for
%   the label map(s) LABELS.
%
%   LABELS is a [M,N,L] UINT32 array containing L label maps (one for
%   each MxN layer). Each label map associates to each of the MxN
%   pixels one of K labels (a value of 0 denotes no association) with
%   unitary mass.
%
%   INTHIST is the [M,N,K] integral histogram obtained by integrating
%   the label maps (an integral histogram is just an array of K
%   integral images, one for each of the K labels). All layers of
%   labels are accumulated to the same integral histogram.
%
%   The total number of labels K is obtained as the maximum value of
%   LABELS. VL_INTHIST(..., 'NUMLABELS', K) specifies the number of
%   labels explicitly.
%
%   VL_INTHIST(..., 'MASS', MASSES) specifies a mass MASSES for each
%   entry of LABELS. MASSES can be either of class UINT32 or DOUBLE,
%   and the class of INTHIST varies accordingly.
%
%   See also: VL_IMINTEGRAL(), VL_SAMPLEINTHIST(), VL_HELP().

% Authors: Andrea Vedaldi
