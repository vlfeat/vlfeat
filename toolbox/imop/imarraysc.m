function J = imarraysc(A, varargin)
% IMARRAYSC  Scale and flattens image array
%   J = IMARRAYSC(A) behaves as IMARRAY(A), but scales the range of
%   each image to span the current colormap.
%
%   IMARRAYSC(...) displays the resultint image rather than
%   returing it.
%
%   IMARRAYSC() works only with indexed (or gray-scale) images.
%
%   In addition to the option-value paris accepted by IMARRAY(),
%   the function accepts also:
%
%   'CLim' [[]]:: 
%     If not empty, use the specified intensity range.
%
%   See also IMARRAY().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

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
  error('IMARRAYSC does not work with true color images') ;
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
  imarray(A,varargin{:}) ;
else
  J = imarray(A,varargin{:}) ;
end
