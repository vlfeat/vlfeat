% VL_SIFTDESCRIPTOR   Run VL_SIFT descriptor on raw data
%  D = VL_SIFTDESCRIPTOR(GRAD, F) calculates the VL_SIFT descriptors of
%  the keypoints F on the pre-processed image GRAD. VL_GRAD is a 2xMxN
%  array. The first layer VL_GRAD(1,:,:) contains the modulus of gradient
%  of the original image modulus. The second layer VL_GRAD(2,:,:)
%  contains the gradient angle (measured in radians, clockwise,
%  starting from the X axis -- this assumes that the Y axis points
%  down). The matrix F contains one column per keypoint with the X, Y,
%  SGIMA and ANLGE parameters.
%
%  In order to match the standard VL_SIFT descriptor, the gradient should
%  be calculated on an image convolved by a Gaussian kernel of
%  variance equal to the scale of the keypoint.  VL_SIFT() also assumes
%  that the input image is pre-smoothed at scale 0.5 (this roughly
%  compensates for the effect of the CCD integrators). The following
%  code computes a standard VL_SIFT descriptor by using VL_SIFTDESCRIPTOR():
%
%    I_       = vl_imsmooth(im2double(I), sqrt(f(3)^2 - 0.5^2)) ;
%    [Ix, Iy] = vl_grad(I_) ;
%    mod      = sqrt(Ix.^2 + Iy.^2) ;
%    ang      = atan2(Iy,Ix) ;
%    grd      = shiftdim(cat(3,mod,ang),2) ;
%    grd      = single(grd) ;
%    d        = vl_siftdescriptor(grd, f) ;
%
%  Remark:: In practice VL_SIFT() samples finitely the scale space and
%    downsamples the image at each octave. Therefore the above
%    procedure may not result in descriptor identical to the one
%    returned by VL_SIFT().
%
%  Options:
%
%  Magnif [3]::
%    Magnification factor (see VL_SIFT()).
%
%  See also:: VL_SIFT(), VL_HELP().

% AUTORIGHTS
