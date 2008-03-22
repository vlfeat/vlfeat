% SIFTDESCRIPTOR   Run SIFT descriptor on raw data
%  D = SIFTDESCRIPTOR(GRAD, F) calculates the SIFT descriptors of
%  keypoints F on the image GRAD. GRAD is a 2xMxN array. The first
%  layer GRAD(1,:,:) contains the image gradient modulus and the
%  second layer GRAD(2,:,:) the image gradient angle (w.r.t. the X
%  axis, clockwise in the convention with the Y axis pointing
%  downs). The matrix F contains one column per keypoint with the X,
%  Y, SGIMA and ANLGE parameters.
%
%  In order to simulate accurately the normal operations performed by
%  SIFT, the gradient should be calculated on an image convolved by a
%  Gaussian kernel of variance equal to the scale of the
%  keypoint. Notice also that SIFT() assumest that the input image is
%  pre-smoothed at scale 0.5 (to compensate for CCD
%  integrators). The following code would compute a "correct" SIFT
%  descriptor
%
%    I_       = imsmooth(im2double(I), sqrt(f(3)^2 - 0.5^2)) ;
%    [Ix, Iy] = grad(I_) ;
%    mod      = sqrt(Ix.^2 + Iy.^2) ;
%    ang      = atan2(Iy,Ix) ;
%    grd      = shiftdim(cat(3,mod,ang),2) ;
%    grd      = single(grd) ;
%    d        = siftdescriptor(grd, f) ;
%
%  Remark:: In practice SIFT() samples finitely the scale space and
%    downsamples the image at each octave. Therefore the above
%    procedure may not result in descriptor identical to the one
%    returned by SIFT().
%
%  See also:: SIFT(), HELP_VLFEAT().

% AUTORIGHTS