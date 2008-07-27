% DFT Dense Feature Transform
%  [F,D] = DFT(I) calculates dense SIFT descriptors for the image I.
%  I must be grayscale in SINGLE format. 
%
%  Step:: [1]
%    Extract a patch every STEP pixels.
%
%  Size:: [3]
%    A spatial bin has a side of SIZE pixels.
%
%  Fast::
%    Use a flat rather than Gaussian window. Much faster.
%
%  Verbose::
%    Be verbose.

% AUTORIGHTS
