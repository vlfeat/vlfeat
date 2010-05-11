function [Ix,Iy] = vl_grad(I,varargin) ;
% VL_GRAD Compute the gradient of an image
%   [Ix,Iy] = VL_GRAD(I) returns the finite differences gradient of
%   the image I. I must be a 2D array. The function uses central
%   differences and for all but the boundaries pixels, for which it
%   uses forward/backward differences as appropriate.
%
%   VL_GRAD(I,'forward') uses only forward differences (except on the
%   lower and right boundaries, where backward difference is used).
%   Similarly, VL_GRAD(I,'backward') uses only backward differences.
%
%   See also: GRADIENT(), VL_HELP().

% Author: Andrea Vedaldi

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

opts.type = 'central' ;
opts = vl_argparse(opts, varargin) ;

[M,N]=size( F ) ;

switch opts.type
  case 'central'
    Ix = [ I(:,2)-I(:,1) , ...
           0.5*(I(:,3:end)-I(:,1:end-2)) , I(:,end)-I(:,end-1) ] ;
    Iy = [ I(2,:)-I(1,:) ; ...
           0.5*(I(3:end,:)-I(1:end-2,:)) ; I(end,:)-I(end-1,:) ] ;
  case 'forward'
    Ix = [ I(:,2:end)-I(:,1:end-1) , I(:,end)-I(:,end-1) ] ;
    Iy = [ I(2:end,:)-I(1:end-1,:) ; I(end,:)-I(end-1,:) ] ;
  case 'backward'
    Ix = [ I(:,2)-I(:,1) , I(:,2:end)-I(:,1:end-1)] ;
    Iy = [ I(2,:)-I(1,:) ; I(2:end,:)-I(1:end-1,:)] ;
  otherwise
    error('Difference type ''%s'' unknown.', opts.type) ;
end

