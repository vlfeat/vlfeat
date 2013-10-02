function h=vl_plotsiftdescriptor(d,f,varargin)
% VL_PLOTSIFTDESCRIPTOR   Plot SIFT descriptor
%   VL_PLOTSIFTDESCRIPTOR(D) plots the SIFT descriptor D. If D is a
%   matrix, it plots one descriptor per column. D has the same format
%   used by VL_SIFT().
%
%   VL_PLOTSIFTDESCRIPTOR(D,F) plots the SIFT descriptors warped to
%   the SIFT frames F, specified as columns of the matrix F. F has the
%   same format used by VL_SIFT().
%
%   H=VL_PLOTSIFTDESCRIPTOR(...) returns the handle H to the line
%   drawing representing the descriptors.
%
%   The function assumes that the SIFT descriptors use the standard
%   configuration of 4x4 spatial bins and 8 orientations bins. The
%   following parameters can be used to change this:
%
%   NumSpatialBins:: 4
%     Number of spatial bins in both spatial directions X and Y.
%
%   NumOrientationBins:: 8
%     Number of orientation bis.
%
%   MagnificationFactor:: 3
%     Magnification factor. The width of one bin is equal to the scale
%     of the keypoint F multiplied by this factor.
%
%   See also: VL_SIFT(), VL_PLOTFRAME(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.magnificationFactor = 3.0 ;
opts.numSpatialBins = 4 ;
opts.numOrientationBins = 8 ;
opts.maxValue = 0 ;

if nargin > 1
  if ~ isnumeric(f)
    error('F must be a numeric type (use [] to leave it unspecified)') ;
  end
end

opts = vl_argparse(opts, varargin) ;

% --------------------------------------------------------------------
%                                                  Check the arguments
% --------------------------------------------------------------------

if(size(d,1) ~= opts.numSpatialBins^2 * opts.numOrientationBins)
  error('The number of rows of D does not match the geometry of the descriptor') ;
end

if nargin > 1
  if (~isempty(f) & (size(f,1) < 2 | size(f,1) > 6))
    error('F must be either empty of have from 2 to six rows.');
  end

  if size(f,1) == 2
    % translation only
    f(3:6,:) = deal([10 0 0 10]') ;
    %f = [f; 10 * ones(1, size(f,2)) ; 0 * zeros(1, size(f,2))] ;
  end

  if size(f,1) == 3
    % translation and scale
    f(3:6,:) = [1 0 0 1]' * f(3,:) ;
    %f = [f; 0 * zeros(1, size(f,2))] ;
  end

  if size(f,1) == 4
    c = cos(f(4,:)) ;
    s = sin(f(4,:)) ;
    f(3:6,:) = bsxfun(@times, f(3,:), [c ; s ; -s ; c]) ;
  end

  if size(f,1) == 5
    assert(false) ;
    c = cos(f(4,:)) ;
    s = sin(f(4,:)) ;
    f(3:6,:) = bsxfun(@times, f(3,:), [c ; s ; -s ; c]) ;
  end

  if(~isempty(f) & size(f,2) ~= size(d,2))
    error('D and F have incompatible dimension') ;
  end
end

% Descriptors are often non-double numeric arrays
d = double(d) ;
K = size(d,2) ;

if nargin < 2 | isempty(f)
  f = repmat([0;0;1;0;0;1],1,K) ;
end

% --------------------------------------------------------------------
%                                                           Do the job
% --------------------------------------------------------------------

xall=[] ;
yall=[] ;

for k=1:K
  [x,y] = render_descr(d(:,k), opts.numSpatialBins, opts.numOrientationBins, opts.maxValue) ;
  xall = [xall opts.magnificationFactor*f(3,k)*x + opts.magnificationFactor*f(5,k)*y + f(1,k)] ;
  yall = [yall opts.magnificationFactor*f(4,k)*x + opts.magnificationFactor*f(6,k)*y + f(2,k)] ;
end

h=line(xall,yall) ;

% --------------------------------------------------------------------
function [x,y] = render_descr(d, numSpatialBins, numOrientationBins, maxValue)
% --------------------------------------------------------------------

% Get the coordinates of the lines of the SIFT grid; each bin has side 1
[x,y] = meshgrid(-numSpatialBins/2:numSpatialBins/2,-numSpatialBins/2:numSpatialBins/2) ;

% Get the corresponding bin centers
xc = x(1:end-1,1:end-1) + 0.5 ;
yc = y(1:end-1,1:end-1) + 0.5 ;

% Rescale the descriptor range so that the biggest peak fits inside the bin diagram
if maxValue
    d = 0.4 * d / maxValue ;
else
    d = 0.4 * d / max(d(:)+eps) ;
end

% We scramble the the centers to have them in row major order
% (descriptor convention).
xc = xc' ;
yc = yc' ;

% Each spatial bin contains a star with numOrientationBins tips
xc = repmat(xc(:)',numOrientationBins,1) ;
yc = repmat(yc(:)',numOrientationBins,1) ;

% Do the stars
th=linspace(0,2*pi,numOrientationBins+1) ;
th=th(1:end-1) ;
xd = repmat(cos(th), 1, numSpatialBins*numSpatialBins) ;
yd = repmat(sin(th), 1, numSpatialBins*numSpatialBins) ;
xd = xd .* d(:)' ;
yd = yd .* d(:)' ;

% Re-arrange in sequential order the lines to draw
nans = NaN * ones(1,numSpatialBins^2*numOrientationBins) ;
x1 = xc(:)' ;
y1 = yc(:)' ;
x2 = x1 + xd ;
y2 = y1 + yd ;
xstars = [x1;x2;nans] ;
ystars = [y1;y2;nans] ;

% Horizontal lines of the grid
nans = NaN * ones(1,numSpatialBins+1);
xh = [x(:,1)' ; x(:,end)' ; nans] ;
yh = [y(:,1)' ; y(:,end)' ; nans] ;

% Verical lines of the grid
xv = [x(1,:) ; x(end,:) ; nans] ;
yv = [y(1,:) ; y(end,:) ; nans] ;

x=[xstars(:)' xh(:)' xv(:)'] ;
y=[ystars(:)' yh(:)' yv(:)'] ;
