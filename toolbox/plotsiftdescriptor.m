function h=plotsiftdescriptor(d,f)
% PLOTSIFTDESCRIPTOR   Plot SIFT descriptor
%   PLOTSIFTDESCRIPTOR(D) plots the SIFT descriptors D, stored as
%   columns of the matrix D. D has the same format used by SIFT().
%
%   PLOTSIFTDESCRIPTOR(D,F) plots the SIFT descriptors warped to the
%   SIFT frames F, specified as columns of the matrix F. F has
%   the same format used by SIFT().
%
%   H=PLOTSIFTDESCRIPTOR(...) returns the handle H to the line drawing
%   representing the descriptors.
%
%   REMARK. Currently the function supports only descriptors with 4x4
%   spatial bins and 8 orientation bins (Lowe's default.)
%
%   See also PLOTSIFTFRAME(), PLOTMATCHES(), PLOTSS().

% AUTORIGHTS
% Copyright (c) 2006 The Regents of the University of California.
% All Rights Reserved.
% 
% Created by Andrea Vedaldi
% UCLA Vision Lab - Department of Computer Science
% 
% Permission to use, copy, modify, and distribute this software and its
% documentation for educational, research and non-profit purposes,
% without fee, and without a written agreement is hereby granted,
% provided that the above copyright notice, this paragraph and the
% following three paragraphs appear in all copies.
% 
% This software program and documentation are copyrighted by The Regents
% of the University of California. The software program and
% documentation are supplied "as is", without any accompanying services
% from The Regents. The Regents does not warrant that the operation of
% the program will be uninterrupted or error-free. The end-user
% understands that the program was developed for research purposes and
% is advised not to rely exclusively on the program for any reason.
% 
% This software embodies a method for which the following patent has
% been issued: "Method and apparatus for identifying scale invariant
% features in an image and use of same for locating an object in an
% image," David G. Lowe, US Patent 6,711,293 (March 23,
% 2004). Provisional application filed March 8, 1999. Asignee: The
% University of British Columbia.
% 
% IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
% FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
% INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND
% ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN
% ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. THE UNIVERSITY OF
% CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
% LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
% A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
% BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE
% MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

lowe_compatible = 1 ;

% --------------------------------------------------------------------
%                                                  Check the arguments
% --------------------------------------------------------------------

if(size(d,1) ~= 128)
  error('D should be a 128xK matrix (only standard descriptors accepted)') ;
end

if nargin > 1
  if(size(f,1) ~= 4)
    error('F should be a 4xK matrix');
  end
  
  if(size(f,2) ~= size(f,2))
    error('D and F must have the same number of columns') ;
  end
end

% Descriptors are often non-double numeric arrays
d = double(d) ;
K = size(d,2) ;
if nargin < 2
  f = repmat([0;0;1;0],1,K) ;
end

maginf = 3.0 ;
NBP=4 ;
NBO=8 ;

% --------------------------------------------------------------------
%                                                           Do the job
% --------------------------------------------------------------------

xall=[] ;
yall=[] ;

for k=1:K
  SBP = maginf * f(3,k) ;
  th=f(4,k) ;
  c=cos(th) ;
  s=sin(th) ;

  [x,y] = render_descr(d(:,k)) ;
  xall = [xall SBP*(c*x-s*y)+f(1,k)] ;
  yall = [yall SBP*(s*x+c*y)+f(2,k)] ;
end

h=line(xall,yall) ;

% --------------------------------------------------------------------
%                                                     Helper functions
% --------------------------------------------------------------------

% Renders a single descriptor
function [x,y] = render_descr( d )

lowe_compatible=1; 
NBP=4 ;
NBO=8 ;

[x,y] = meshgrid(-NBP/2:NBP/2,-NBP/2:NBP/2) ;

% Rescale d so that the biggest peak fits inside the bin diagram
d = 0.4 * d / max(d(:)) ;

% We have NBP*NBP bins to plot. Here are the centers:
xc = x(1:end-1,1:end-1) + 0.5 ;
yc = y(1:end-1,1:end-1) + 0.5 ;

% We swap the order of the bin diagrams because they are stored row
% major into the descriptor (Lowe's convention that we follow.)
xc = xc' ;
yc = yc' ;

% Each bin contains a star with eight tips
xc = repmat(xc(:)',NBO,1) ;
yc = repmat(yc(:)',NBO,1) ;

% Do the stars 
th=linspace(0,2*pi,NBO+1) ;
th=th(1:end-1) ;
if lowe_compatible
  xd = repmat(cos(-th), 1, NBP*NBP ) ;
  yd = repmat(sin(-th), 1, NBP*NBP ) ;
else
  xd = repmat(cos(th), 1, NBP*NBP ) ;
  yd = repmat(sin(th), 1, NBP*NBP ) ;
end
xd = xd .* d(:)' ;
yd = yd .* d(:)' ;

% Re-arrange in sequential order the lines to draw
nans = NaN * ones(1,NBP^2*NBO) ;
x1 = xc(:)' ;
y1 = yc(:)' ;
x2 = x1 + xd ;
y2 = y1 + yd ;
xstars = [x1;x2;nans] ;
ystars = [y1;y2;nans] ;

% Horizontal lines of the grid
nans = NaN * ones(1,NBP+1);
xh = [x(:,1)' ; x(:,end)' ; nans] ;
yh = [y(:,1)' ; y(:,end)' ; nans] ;

% Verical lines of the grid
xv = [x(1,:) ; x(end,:) ; nans] ;
yv = [y(1,:) ; y(end,:) ; nans] ;

x=[xstars(:)' xh(:)' xv(:)'] ;
y=[ystars(:)' yh(:)' yv(:)'] ;
