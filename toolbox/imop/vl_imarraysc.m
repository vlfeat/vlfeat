function J = vl_imarraysc(A, varargin)
% VL_IMARRAYSC  Scale and flattens image array
%   J = VL_IMARRAYSC(A) behaves as VL_IMARRAY(A), but scales the range of
%   each image to span the current colormap.
%
%   VL_IMARRAYSC(...) displays the resulting image rather than
%   returing it.
%
%   VL_IMARRAYSC() works only with indexed (or gray-scale) images.
%
%   VL_IMARRAYSC() accepts the options of VL_IMARRAY() and:
%
%   CLim [[]]:: 
%     Specify the intensity range. If empty, the range is
%     calcualted automatically.
%
%   See also:: VL_IMARRAY(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

reverse=1 ;
spacing=0 ;
clim=[] ;
cmap=colormap ;

for k=1:2:length(varargin)
  opt=varargin{k} ;
  arg=varargin{k+1} ;
  switch lower(varargin{k})
    case 'cmap'
      cmap=arg ;
    case 'clim'
      clim=arg;
    otherwise
  end
end

if ndims(A) > 3
  error('VL_IMARRAYSC does not work with true color images') ;
end

if isinteger(A)
	A = single(A) ;
end

% rescale
tmp = A ;
K = size(A,3) ;
L = size(cmap,1) ;

for k=1:K

    if isempty(clim)
      tmp = A(:,:,k) ;
      m = min(tmp(:)) ;
      M = max(tmp(:)) ;
    else
      m = clim(1) ;
      M = clim(2) ;
    end
    
    a = L / (M-m+eps) ;
    b = .5 - m*a ;
    A(:,:,k) = max(min(a * A(:,:,k) + b,L),1) ;
		
end

if nargout == 0 
  vl_imarray(A,varargin{:}) ;
else
  J = vl_imarray(A,varargin{:}) ;
end
