function J = vl_imarray(A,varargin)
% VL_IMARRAY  Flattens image array
%   J=VL_IMARRAY(A) flattens the array of images A. A can be either a
%   M*N*K array, storing one gray-scale image per slice, or a M*N*3*K
%   or M*N*K*3 array, storing one RGB image per slice.  The function
%   returns an image J which is a tiling of the images in the array.
%
%   VL_IMARRAY(...) display the image J rather than returning it.
%
%   VL_IMARRAY() accepts the following options:
%
%   'Spacing' [0]::
%     Orlates the images with a null border of the specified width.
%
%   'Layout' [[]]::
%     Specify a vector [TM TN] with the number of rows and columns of
%     the tiling. If equal to [] the layout is computed automatically.
%
%   'Movie' [0]::
%     Display/returns a movie rather than generating a tyling.
%
%   'CMap' [[]]::
%     Specify a colormap for indexed images and movies.
%
%   See also VL_IMARRAYSC(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

reverse = 1 ;
sp      = 0 ;
lay     = [] ;
swap3   = 0 ;
domov   = 0 ;
cmap    = colormap ;

% process options
for k=1:2:length(varargin)  
  opt = varargin{k} ;
  arg = varargin{k+1} ;
  switch lower(opt)
    case 'layout'
      lay=arg;
    case 'spacing'
      sp=arg;
    case 'movie'
      domov=arg;
    case 'cmap'
      cmap=arg; 
    case 'clim'      
    otherwise
      error(sprintf('Unknown option ''%s''',opt)) ;
  end
end

% retrieve image dimensions
if ndims(A) <= 3
  d=1 ;
  [Mi,Ni,K] = size(A) ;
else
  if size(A,3) == 3
    [Mi,Ni,d,K] = size(A) ;
  elseif size(A,4) == 3 ;
    swap3 = 1 ;
    [Mi,Ni,K,d] = size(A) ;
  else
    error(['A should be either M*N*K or M*N*3*K or M*N*K*3']);
  end
end

% compute layout
if isempty(lay)
  N = ceil(sqrt(K)) ;
  M = ceil(K/N) ;
else
  M = lay(1) ;
  N = lay(2) ;
  K = min(K,M*N) ; 
end

% make storage
if ~ domov
  cdata = zeros(Mi*M + sp*(M-1), Ni*N + sp*(N-1), d, class(A)) ;
end

% add one image per time
for k=1:K

  % retriee k-th image
  if(d == 1)
    tmp = A(:,:,k) ;
  else
    if swap3
      tmp = A(:,:,k,:) ;
    else
      tmp = A(:,:,:,k) ;
    end
  end
  
  if ~ domov    
    p = k - 1 ;
    i = floor(p/N) ;
    if reverse
      i = M-1 - i ;
    end
    j = mod(p,N) ;
    irng = i*(Mi+sp) + (0:Mi-1) + 1 ;
    jrng = j*(Ni+sp) + (0:Ni-1) + 1 ;    
    cdata(irng,jrng,:) = tmp ;
  else
    MOV(k) = im2frame(tmp,cmap) ;
  end
end

if ~ domov
  if nargout == 0
    image(cdata) ; 
    colormap(cmap) ;
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
