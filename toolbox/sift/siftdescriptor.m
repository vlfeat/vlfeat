% SIFTDESCRIPTOR   Run SIFT descriptor on raw data
%  D = SIFTDESCRIPTOR(GRAD, F) calculates the SIFT descriptors of
%  thekeypoints F on the image GRAD. GRAD is a 2xMxN array. The first
%  layer GRAD(1,:,:) contains the image gradient modulus and the
%  second layer GRAD(2,:,:) the image gradient angle (w.r.t. the X
%  axis, clockwise, Y axis pointing downwards). The matrix F contains
%  one column per keypoint with the X, Y, SGIMA and ANLGE parameters.
%
%  In order to match the standard SIFT descriptor, the gradient should
%  be calculated on an image convolved by a Gaussian kernel of
%  variance equal to the scale of the keypoint.  SIFT() also assumes
%  that the input image is pre-smoothed at scale 0.5 (this roughly
%  compensates for the effect of the CCD integrators). The following
%  code computes a standard SIFT descriptor by using SIFTDESCRIPTOR():
%
%    I_       = imsmooth(im2double(I), sqrt(f(3)^2 - 0.5^2)) ;
%    [Ix, Iy] = grad(I_) ;
%    mod      = sqrt(Ix.^2 + Iy.^2) ;
%    ang      = atan2(Iy,Ix) ;
%    grd      = shiftdim(cat(3,mod,ang),2) ;
%    grd      = single(grd) ;
%    d        = siftdescriptor(grd, f) ;
%
%  REMARK. Notice that, in order to speed up the calculations, SIFT()
%  samples the scale parameter at discrete steps and downsamples the
%  image at each octave. Therefore the above procedure may not result
%  in descriptor identical to the ones returned by SIFT().
%
%  Options:
%
%  MAGNIF::
%    Magnification factor (see SIFT()).
%
%  See also HELP_VLFEAT(), SIFT().

% AUTORIGHTS
