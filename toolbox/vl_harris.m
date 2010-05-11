function [H,details] = vl_harris(I,si,alpha)
% VL_HARRIS  Harris corner strength
%  H = VL_HARRIS(I,SI) computes the Harris corner strength of the image I
%  at ``integration'' scale SI.
%
%  The Harris strength [1] of a pixel is a statistic of the gradient
%  of the image integrated in a neighborhood of that pixel. This
%  neighborhood is a Gaussian window of variance SI.
%
%  In computing the Harris corner strength, there is a second scale
%  parameter, the ``derivation'' scale SD, which is the variance of
%  the Gaussian kernel used to pre-smooth the image I before computing
%  its gradient. SI and SD are independent parameters and VL_HARRIS(I,SI)
%  assumes that I is already smoothed at level SD.
%
%  VL_HARRIS(I,SI) uses Noble's variation [2] of the Harris score. If
%  SIGMAP and SIGMAM are respectively the biggest and smallest
%  eigenvalue of the structure tensor at a pixel, the score is given
%  by (SIGMAP*SIGMAM) / (SIGMAP+SIGMAM/2).  Let GAMMA = SIGMAM/SIGMAP
%  the ratio between the eigenvalues, which measures the degree of
%  anisotropy of the tensor and is always comprised in the range
%  [0,1].  Noble's score can be decomposed in two factors: the biggest
%  eigenvalue SIGMAP and the number
%
%    RHO = (2 GAMMA) / (GAMMA + 1).
%
%  RHO is another measure of isotropy that has value one for a
%  symmetric tensor and and zero for maximally anisotropic tensor.
%  [H,DETAILS] = VL_HARRIS(I,SIGMA) returns the additional structure
%  DETAILS with the following fields:
%
%    DETAILS.SIGMAP
%    DETAILS.RHO
%
%  VL_HARRIS(I,SI,ALPHA) uses Harris' original score [1], defined to be
%  SIGMAP*SIGMAM - ALPHA*(SIGMAP+SIGMAM)^2. This can be decomposed in
%  the factors SIGMAP^2 (note the square) and
%  
%    RHO = GAMMA - ALPHA (1+GAMMA)^2.
%
%  Note that RHO is equal to -ALPHA for a maximally anisotropic
%  tensor. Typically ALPHA=0.04 and this is what is used by
%  VL_HARRIS(I,SI,[]).
%
%  REMARK. The gradient of the image I, used to compute the structure
%  tensor, is computed using central differencies. This means that a
%  function line [+1,-1,+1,...] has null Harris' score. This is
%  generally assumed to be a sampling artifact, and might be
%  avoided by oversampling the image.
%
%  EXAMPLE:: 
%    To extacts Harris points from image I:
%      idx = vl_localmax( vl_harris( vl_imsmooth( I, sd ), si ) ) ;
%      [i,j] = ind2sub( size(I), idx )
%
%  REFERENCES::
%   [1] C. Harris and M. Stephens, "A combined corner and edge detector,"
%   in Proceedings of The Fourth Alvey Vision Conference, pp. 147-151,
%   1988.
%
%   [2] J. A. Noble, "Finding corners, "Image Vision Computing, vol. 6,
%   no. 2, pp. 121-128, 1988.
%
%  See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

[Ix,Iy] = gradient(I) ;
H11 = vl_imsmooth(Ix.*Ix, si) ;
H12 = vl_imsmooth(Ix.*Iy, si) ;
H22 = vl_imsmooth(Iy.*Iy, si) ;

if nargin > 2
  % Harris' original
  if isempty(alpha)
    alpha = 0.04 ;
  end
  H = (H11.*H22 - H12.^2) - alpha*(H11+H22).^2 ;
else
  % Noble's variation
  H = 2 *  (H11.*H22 - H12.^2)./(H11+H22+eps) ;
end

% -------------------------------------------------------------------
%                                              Extract auxiliary info
% -------------------------------------------------------------------

if nargout > 1
  tr = H11 + H22 ;
  dt = H11.*H22 - H12.^2 ;
  Lm = 0.5 * (tr - sqrt(tr.^2 - 4*dt));
  Lp = 0.5 * (tr + sqrt(tr.^2 - 4*dt));
  Lm = real(Lm) ;
  Lp = real(Lp) ;
  
  gamma=sqrt(Lm./Lp) ;

  details.sigmap = Lp ;
  if nargin > 2 
    details.rho = gamma - alpha * (1+gamma).^2 ;
  else
    details.rho   = 2*gamma ./ (1 + gamma) ;
  end
end
