function J = vl_imarray(A,varargin)
% VL_IMARRAY  Flattens image array
%   J=VL_IMARRAY(A) creates a mosaic J from the array of images A. A
%   can be either a M*N*K array, storing one gray-scale image per
%   slice, or a M*N*3*K or M*N*K*3 array, storing a true color RGB
%   image per slice. The function returns an image J which is a tiling
%   of the images in the array. Tiles are filled from left to right
%   and top to bottom.
%
%   VL_IMARRAY(...) displays the image J rather than returning it.
%
%   VL_IMARRAY() accepts the following options:
%
%   Spacing:: 0
%     Separate the images by a border of the specified width (the
%     border is assigned 0 value, which usually corresponds to black).
%
%   Layout:: empty
%     Specify a vector [TM TN] with the number of rows and columns of
%     the tiling. If equal to [] the layout is computed automatically.
%
%   Movie:: false
%     Display or return a movie instead of generating a tiling.
%
%   CMap:: []
%     Specify a colormap to construct a movie when the input is an
%     indexed image array. If not specified, MATLAB default colormap
%     is used.
%
%   Reverse:: true
%     Start filling the mosaic tiles from the bottom rather than from
%     the top.
%
%   See also:VL_IMARRAYSC(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.reverse = false ;
opts.spacing = 0 ;
opts.layout = [] ;
opts.movie = false ;
opts.cmap = [] ;
opts = vl_argparse(opts, varargin) ;

swap3 = false ;

% retrieve image dimensions
if ndims(A) <= 3
  numChannels = 1 ;
  [height,width,numImages] = size(A) ;
else
  if ndims(A) == 4 && size(A,3) == 3
    [height,width,numChannels,numImages] = size(A) ;
  elseif ndims(A) == 4 && size(A,4) == 3 ;
    swap3 = true ;
    [height,width,numImages,numChannels] = size(A) ;
  else
    error('A is neither M*N*K, nor M*N*3*K, nor M*N*K*3.') ;
  end
end

% compute layout
if isempty(opts.layout)
  N = ceil(sqrt(numImages)) ;
  M = ceil(numImages / N) ;
else
  M = opts.layout(1) ;
  N = opts.layout(2) ;
  numImages = min(numImages, M*N) ;
end

% make storage for composite image
if ~ opts.movie
  cdata = zeros(height * M + opts.spacing * (M-1), ...
                width  * N + opts.spacing * (N-1), ...
                numChannels, ...
                class(A)) ;
end

% add one image per time
for k = 1:numImages

  % retrieve k-th image
  if numChannels == 1
    tmp = A(:,:,k) ;
  else
    if swap3
      tmp = A(:,:,k,:) ;
    else
      tmp = A(:,:,:,k) ;
    end
  end

  if ~ opts.movie
    p = k - 1 ;
    i = floor(p/N) ;
    if opts.reverse
      i = M - 1 - i ;
    end
    j = mod(p,N) ;
    ir = i * (height + opts.spacing) + (1:height) ;
    jr = j * (width  + opts.spacing) + (1:width) ;
    cdata(ir,jr,:) = tmp ;
  else
    if isempty(opts.cmap)
      opts.cmap = get(0, 'DefaultFigureColormap') ;
    end
    MOV(k) = im2frame(tmp, opts.cmap) ;
  end
end

if ~ opts.movie
  if nargout == 0
    image(cdata) ;
    colormap(opts.cmap) ;
    return ;
  else
    J = cdata ;
  end
else
  if nargout == 0
    movie(MOV) ;
    return ;
  else
    J = MOV ;
  end
end
