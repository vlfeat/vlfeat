function h=vl_plotsiftdescriptor(d,f,varargin)
% VL_PLOTSIFTDESCRIPTOR   Plot SIFT descriptor
%   VL_PLOTSIFTDESCRIPTOR(D) plots the SIFT descriptors D, stored as
%   columns of the matrix D. D has the same format used by VL_SIFT().
%
%   VL_PLOTSIFTDESCRIPTOR(D,F) plots the SIFT descriptors warped to
%   the SIFT frames F, specified as columns of the matrix F. F has the
%   same format used by VL_SIFT().
%
%   H=VL_PLOTSIFTDESCRIPTOR(...) returns the handle H to the line drawing
%   representing the descriptors.
%
%   REMARK. By default, the function assumes descriptors with 4x4
%   spatial bins and 8 orientation bins (Lowe's default.)
%
%   The function supports the following options
%
%   NumSpatialBins:: 4
%     Number of spatial bins in each spatial direction.
%
%   NumOrientBins:: 8
%     Number of orientation bis.
%
%   Magnif:: 3
%     Magnification factor.
%
%   See also VL_HELP(), VL_SIFT(), VL_PLOTFRAME().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

magnif = 3.0 ;
NBP    = 4 ;
NBO    = 8 ;
maxv   = 0 ;

if nargin > 1
  if ~ isnumeric(f)
    error('F must be a numeric type (use [] to leave it unspecified)') ;
  end
end

for k=1:2:length(varargin)
  opt=lower(varargin{k}) ;
  arg=varargin{k+1} ;
  switch opt
    case 'numspatialbins'
      NBP = arg ;
    case 'numorientbins'
      NBO = arg ;
    case 'magnif'
      magnif = arg ;
    case 'maxv'
      maxv = arg ;
    otherwise
      error(sprintf('Unknown option ''%s''', opt)) ;     
  end
end

% --------------------------------------------------------------------
%                                                  Check the arguments
% --------------------------------------------------------------------

if(size(d,1) ~= NBP*NBP*NBO)
  error('The number of rows of D does not match the geometry of the descriptor') ;
end

if nargin > 1
  if (~isempty(f) & size(f,1) < 2 | size(f,1) > 4)
    error('F should be a 2xK, 3xK, 4xK matrix or the empty matrix');
  end
  
  if size(f,1) == 2
    f = [f; 10 * ones(1, size(f,2)) ; 0 * zeros(1, size(f,2))] ;
  end
  
  if size(f,1) == 3
    f = [f; 0 * zeros(1, size(f,2))] ;
  end
  
  if(~isempty(f) & size(f,2) ~= size(d,2))
    error('D and F have incompatible dimension') ;    
  end
end

% Descriptors are often non-double numeric arrays
d = double(d) ;
K = size(d,2) ;

if nargin < 2 | isempty(f)
  f = repmat([0;0;1;0],1,K) ;
end

% --------------------------------------------------------------------
%                                                           Do the job
% --------------------------------------------------------------------

xall=[] ;
yall=[] ;

for k=1:K
  SBP = magnif * f(3,k) ;
  th=f(4,k) ;
  c=cos(th) ;
  s=sin(th) ;

  [x,y] = render_descr(d(:,k), NBP, NBO, maxv) ;
  xall = [xall SBP*(c*x-s*y)+f(1,k)] ;
  yall = [yall SBP*(s*x+c*y)+f(2,k)] ;
end

h=line(xall,yall) ;


% --------------------------------------------------------------------
function [x,y] = render_descr(d, BP, BO, maxv)
% --------------------------------------------------------------------

[x,y] = meshgrid(-BP/2:BP/2,-BP/2:BP/2) ;

% Rescale d so that the biggest peak fits inside the bin diagram
if maxv
    d = 0.4 * d / maxv ;
else
    d = 0.4 * d / max(d(:)+eps) ;
end

% We have BP*BP bins to plot. Here are the centers:
xc = x(1:end-1,1:end-1) + 0.5 ;
yc = y(1:end-1,1:end-1) + 0.5 ;

% We scramble the the centers to have the in row major order
% (descriptor convention).
xc = xc' ;
yc = yc' ;

% Each spatial bin contains a star with BO tips
xc = repmat(xc(:)',BO,1) ;
yc = repmat(yc(:)',BO,1) ;

% Do the stars 
th=linspace(0,2*pi,BO+1) ;
th=th(1:end-1) ;
xd = repmat(cos(th), 1, BP*BP) ;
yd = repmat(sin(th), 1, BP*BP) ;
xd = xd .* d(:)' ;
yd = yd .* d(:)' ;

% Re-arrange in sequential order the lines to draw
nans = NaN * ones(1,BP^2*BO) ;
x1 = xc(:)' ;
y1 = yc(:)' ;
x2 = x1 + xd ;
y2 = y1 + yd ;
xstars = [x1;x2;nans] ;
ystars = [y1;y2;nans] ;

% Horizontal lines of the grid
nans = NaN * ones(1,BP+1);
xh = [x(:,1)' ; x(:,end)' ; nans] ;
yh = [y(:,1)' ; y(:,end)' ; nans] ;

% Verical lines of the grid
xv = [x(1,:) ; x(end,:) ; nans] ;
yv = [y(1,:) ; y(end,:) ; nans] ;

x=[xstars(:)' xh(:)' xv(:)'] ;
y=[ystars(:)' yh(:)' yv(:)'] ;
