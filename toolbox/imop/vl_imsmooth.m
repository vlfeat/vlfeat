% VL_IMSMOOTH  Smooth image
%   J = VL_IMSMOOTH(I,SIGMA) convolves the image I by an isotropic
%   Gaussian kernel of standard deviation SIGMA.  I must be an array
%   of doubles. IF the array is three dimensional, the third dimension
%   is assumed to span different channels (e.g. R,G,B). In this case,
%   each channel is convolved independently.
%
%   VL_IMSMOOTH() accepts the following options:
%
%   Kernel::
%     Selects between GAUSSIAN and TRIANGULAR kernels. The triangular
%     kernel support has 2*SIGMA-1 sampels. Kernels have unit mass.
%
%   Padding::
%     Selects between ZERO or CONTINUITY padding method to handle the
%     image boundaries. ZERO extends the input image with zeroes
%     around the border, and CONTINUITY extends the image with
%     constant pixels.
%
%   Step::
%     Sets the subsampling step. A subsampling step of STEP pixels
%     causes J(1:STEPS:end, 1:STEPS:end, :) to be computed. This is
%     useful to downsample the image.
%
%   See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
