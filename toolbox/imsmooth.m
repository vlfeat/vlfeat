% IMSMOOTH  Smooth image
%   I=IMSMOOTH(I,SIGMA) convolves the image I by an isotropic Gaussian
%   kernel of standard deviation SIGMA.  I must be an array of
%   doubles. IF the array is three dimensional, the third dimension is
%   assumed to span different channels (e.g. R,G,B). In this case,
%   each channel is convolved independently.
%     
%  See also OVERVIEW_VLUTIL().
