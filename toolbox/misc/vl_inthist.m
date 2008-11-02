% VL_INTHIST  Integral histogram
%
%   INTHIST = VL_INTHIST(LABELS) computes the integral histogram for
%   the label maps LABELS. 
%
%   LABELS is a [M,N,L] UINT32 array containing L label maps (one for
%   each MxN layer). Each label map associate to each pixel one of K
%   labels (0 denotes no association).
%
%   INTHIST is the [M,N,K] integral histograms obtaining by
%   integrating the label maps. The total number of labels K is
%   obtained as the maximum value of LABELS, and each enry in a label
%   map contributes with unitary mass.
%
%   VL_INTHIST(..., 'MASS', MASSES) specifies the mass MASSES for each
%   entry of LABELS.
%
%   VL_INTHIST(..., 'NUMLABELS, K) specifies the number of labels
%   explicitly.
%   
%   Author:: Andrea Vedaldi