function J = vl_imarraysc(A, varargin)
% VL_IMARRAYSC  Scale and flattens image array
%   J=VL_IMARRAYSC(A) constructs an image mosaic similar to
%   J=VL_IMARRAY(A), but it rescales the range of each image in the
%   array. If A is an array of grayscale images, J will index all the
%   colors in the current colormap; if A is a true color image, J will
%   span the range [0,1].
%
%   If A is of an integer class, J will be of class single SINGLE class.
%
%   VL_IMARRAYSC(...) displays the image J rather than returning it.
%
%   VL_IMARRAYSC() accepts the options of VL_IMARRAY() and:
%
%   CLim:: []
%     Rescale the specified range of values rather than the actual
%     range of each image.
%
%   Uniform:: [false]
%     Rescale the range of all the images together, rather than on
%     an image-by-image basis.
%
%   CMap:: []
%     Use the specified color map as a reference rather than the
%     current or default one.
%
%   Algorithm::
%     CLim is seet to the image range [m, M], where m is the minimum
%     value of an image and M is the maximum. The image range CLim is
%     then affinely mapped to the integers from 1 to C, where C is the
%     number of colors in the colormap, or to the range [0,1] for true
%     color images. The mapping is done so that the first color is
%     assigned the first subinterval of length C of the range [m,M]
%     and so on.
%
%   See also: VL_IMARRAY(), VL_HELP(), IMAGE().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.clim = [] ;
opts.cmap = colormap ;
opts.uniform = false ;
[opts, varargin] = vl_argparse(opts, varargin) ;

numDimensions = ndims(A)  ;
if numDimensions <= 3
  numChannels = 1 ;
  numImages = size(A,3) ;
  numLevels = size(opts.cmap,1) ;
elseif numDimensions == 4
  numChannels = size(A,3) ;
  numImages = size(A,4) ;
  numLevels = 1 ;
  if numChannels ~= 3
    error('A has more than three dimensions and the third one is not equal to three.') ;
  end
else
  error('A has neither 2, 3 or 4 dimensions.') ;
end

if opts.uniform & ~isempty(opts.clim)
  error('UNIFORM cannot be used in combination with CLIM.') ;
end

if isinteger(A)
  A = single(A) ;
end

if opts.uniform && numDimensions < 4
  opts.clim = [min(A(:)) max(A(:))] ;
end

for k = 1:numImages
  if isempty(opts.clim)
    if numChannels == 1
      tmp = A(:,:,k) ;
    else
      tmp = A(:,:,:,k) ;
    end
    dataMin = min(tmp(:)) ;
    dataMax = max(tmp(:)) ;
  else
    dataMin = opts.clim(1) ;
    dataMax = opts.clim(2) ;
  end
  a = numLevels  / (dataMax - dataMin + eps) ;
  b = - dataMin * a ;
  if numChannels == 1
    A(:,:,k) = max(min(floor(a * A(:,:,k) + b + 1),numLevels),1) ;
  else
    A(:,:,:,k) = max(min(a * A(:,:,:,k) + b,numLevels),0) ;
  end
end

if nargout == 0
  vl_imarray(A,varargin{:}, 'cmap', opts.cmap) ;
else
  J = vl_imarray(A,varargin{:}, 'cmap', opts.cmap) ;
end
