function J = vl_imarraysc(A, varargin)
% VL_IMARRAYSC  Scale and flattens image array
%   J = VL_IMARRAYSC(A) behaves as VL_IMARRAY(A), but scales the range
%   of each image to span the current colormap.
%
%   VL_IMARRAYSC(...) displays the composed image rather than returing
%   it.
%
%   VL_IMARRAYSC() works only with indexed (or gray-scale) images.
%
%   VL_IMARRAYSC() accepts the options of VL_IMARRAY() and:
%
%   CLim:: [empty]
%     Specify the intensity range. If empty, the range is calcualted
%     automatically for each image in the array.
%
%   Uniform:: [false]
%     If true all the images in the array are scaled in the same
%     range.
%
%   See also: VL_IMARRAY(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

opts.clim = [] ;
opts.cmap = colormap ;
opts.uniform = false ;
[opts, varargin] = vl_argparse(opts, varargin) ;

if ndims(A) > 3
  error('A must be an array with at most three dimensions.') ;
end

if opts.uniform & ~isempty(opts.clim)
  error('Only UNIFORM or CLIM can be specified.') ;
end

if isinteger(A)
  A = single(A) ;
end

if opts.uniform
  opts.clim = [min(A(:)) max(A(:))] ;
end

% rescale range
tmp = A ;
K = size(A,3) ;
L = size(opts.cmap,1) ;

for k=1:K
  if isempty(opts.clim)
    tmp = A(:,:,k) ;
    m = min(tmp(:)) ;
    M = max(tmp(:)) ;
  else
    m = opts.clim(1) ;
    M = opts.clim(2) ;
  end
  a = L / (M - m + eps) ;
  b = .5 - m * a ;
  A(:,:,k) = max(min(a * A(:,:,k) + b,L),1) ;
end

if nargout == 0
  vl_imarray(A,varargin{:}, 'cmap', opts.cmap) ;
else
  J = vl_imarray(A,varargin{:}, 'cmap', opts.cmap) ;
end
