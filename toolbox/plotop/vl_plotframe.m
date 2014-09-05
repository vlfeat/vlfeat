function h = vl_plotframe(frames,varargin)
% VL_PLOTFRAME  Plot a geometric frame
%  VL_PLOTFRAME(FRAME) plots the feature frame FRAME. The frame can be
%  either a 2D point, a circle, an oriented circle, an ellipse, or an
%  oriented ellipse, as follows:
%
%  Point::
%    FRAME has 2 components. FRAME(1:2) are the x,y coordinates of the
%    point.
%
%  Circle::
%    FRAME has 3 components. FRAME(1:2) are the x,y coordinates of the
%    center and FRAME(3) is its radius.
%
%  Oriented circle::
%    FRAME has 4 components. FRAME(1:2) are the x,y coordiantes of the
%    center of the circle, FRAME(3) is the radius, and FRAME(4) is the
%    orientation, expressed as a rotation in radians of the standard
%    oriented frame (see below). Positive rotations appear clockwise
%    since the image coordinate system is left-handed.
%
%  Ellipse::
%    FRAME has 5 components. FRAME(1:2) are the x,y coordiantes of the
%    center and FRAME(3:5) are the elements S11, S12, S22 of a 2x2
%    covariance matrix S (a positive semidefinite matrix) defining the
%    ellipse shape. The ellipse is the set of points {x + T: x' inv(S)
%    x = 1}, where T is the ellipse center.
%
%  Oriented ellipse::
%    FAME has 6 components. FRAME(1:2) are the coordiantes T=[x;y] of
%    the center. FRAME(3:6) is the column-wise stacking of a 2x2
%    matrix A. The oriented ellipse is obtained by applying the affine
%    transformation (A,T) to the standard oriented frame (see below).
%
%  A standard unoriented frame is a circle of unit radius centered at
%  the origin; a standard oriented frame is the same, but marked with
%  a radius pointing towards the positive Y axis (i.e. downwards
%  according to the standard MATLAB image reference frame) to
%  represent the frame orientation. All other frames can be obtained
%  as affine transformations of these two. In the case of unoriented
%  frames, this transformation is ambiguous up to a rotation.
%
%  VL_PLOTFRAME(FRAMES), where FRAMES is a D x N matrix, plots N
%  frames, one per column. This is significantly more efficient than
%  looping over frames explicitly.
%
%  H = VL_PLOTFRAME(...) returns the handle H of the graphical object
%  representing the frames.
%
%  VL_PLOTFRAME(FRAMES,...) passes any extra argument to the
%  underlying plotting function. The first optional argument, in
%  particular, can be a line specification string such as the one used
%  by PLOT().
%
%  See also: <a href="matlab:vl_help('sift')">SIFT</a>,
%  <a href="matlab:vl_help('covdet')">covariant detectors</a>,
%  VL_FRAME2OELL(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% Copyright (C) 2013 Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% number of vertices drawn for each frame
np = 40 ;

lineprop = {} ;
if length(varargin) > 0
  lineprop = vl_linespec2prop(varargin{1}) ;
  lineprop = {lineprop{:}, varargin{2:end}} ;
end

% --------------------------------------------------------------------
%                                         Handle various frame classes
% --------------------------------------------------------------------

% if just a vector, make sure it is column
if(min(size(frames))==1)
  frames = frames(:) ;
end

[D,K] = size(frames) ;
zero_dimensional = D==2 ;

% just points?
if zero_dimensional
  h = plot(frames(1,:),frames(2,:),'g.',lineprop{:}) ;
  return ;
end

% reduce all other cases to ellipses/oriented ellipses
frames = vl_frame2oell(frames) ;
do_arrows = (D==4 || D==6) ;

% --------------------------------------------------------------------
%                                                                 Draw
% --------------------------------------------------------------------

K   = size(frames,2) ;
thr = linspace(0,2*pi,np) ;

% allx and ally are nan separated lists of the vertices describing the
% boundary of the frames
allx = nan*ones(1, np*K+(K-1)) ;
ally = nan*ones(1, np*K+(K-1)) ;

if do_arrows
  % allxf and allyf are nan separated lists of the vertices of the
  allxf = nan*ones(1, 3*K) ;
  allyf = nan*ones(1, 3*K) ;
end

% vertices around a unit circle
Xp = [cos(thr) ; sin(thr) ;] ;

for k=1:K
  % frame center
  xc = frames(1,k) ;
  yc = frames(2,k) ;

  % frame matrix
  A = reshape(frames(3:6,k),2,2) ;

  % vertices along the boundary
  X = A * Xp ;
  X(1,:) = X(1,:) + xc ;
  X(2,:) = X(2,:) + yc ;

  % store
  allx((k-1)*(np+1) + (1:np)) = X(1,:) ;
  ally((k-1)*(np+1) + (1:np)) = X(2,:) ;

  if do_arrows
    allxf((k-1)*3 + (1:2)) = xc + [0 A(1,2)] ;
    allyf((k-1)*3 + (1:2)) = yc + [0 A(2,2)] ;
  end
end

if do_arrows
  h = line([allx nan allxf], ...
           [ally nan allyf], ...
           'Color','g','LineWidth',3, ...
           lineprop{:}) ;
else
  h = line(allx, ally, ...
           'Color','g','LineWidth',3, ...
           lineprop{:}) ;
end



