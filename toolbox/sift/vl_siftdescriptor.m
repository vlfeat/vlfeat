% VL_SIFTDESCRIPTOR  Raw SIFT descriptor
%   D = VL_SIFTDESCRIPTOR(GRAD, F) calculates the SIFT descriptors of
%   the keypoints F on the pre-processed image GRAD. GRAD is a 2xMxN
%   array. The first layer GRAD(1,:,:) contains the modulus of
%   gradient of the original image modulus. The second layer
%   GRAD(2,:,:) contains the gradient angle (measured in radians,
%   clockwise, starting from the X axis -- this assumes that the Y
%   axis points down). The matrix F contains one column per keypoint
%   with the X, Y, SIGMA and ANGLE parameters.
%
%   In order to match the standard SIFT descriptor, the gradient GRAD
%   should be calculated after mapping the image to the keypoint
%   scale. This is obtained by smoothing the image by a a Gaussian
%   kernel of variance equal to the scale of the keypoint.
%   Additionaly, SIFT assumes that the input image is pre-smoothed at
%   scale 0.5 (this roughly compensates for the effect of the CCD
%   integrators), so the amount of smoothing that needs to be applied
%   is slightly less. The following code computes a standard SIFT
%   descriptor by using VL_SIFTDESCRIPTOR():
%
%     I_       = vl_imsmooth(im2double(I), sqrt(f(3)^2 - 0.5^2)) ;
%     [Ix, Iy] = vl_grad(I_) ;
%     mod      = sqrt(Ix.^2 + Iy.^2) ;
%     ang      = atan2(Iy,Ix) ;
%     grd      = shiftdim(cat(3,mod,ang),2) ;
%     grd      = single(grd) ;
%     d        = vl_siftdescriptor(grd, f) ;
%
%   Remark::
%     The above fragment generates results which are very close
%     but not identical to the output of VL_SIFT() as the latter
%     samples the scale space at finite steps.
%
%   Remark::
%     For object categorization is sometimes useful to compute
%     SIFT descriptors without smoothing the image.
%
%   Options:
%
%   Magnif:: [3]
%     Magnification factor (see VL_SIFT()).
%
%   NormThresh:: [-inf]
%     Set the minimum l2-norm of the descriptors before
%     normalization. Descriptors below the threshold are set to zero.
%
%  See also: VL_SIFT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
