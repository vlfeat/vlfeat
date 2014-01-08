function eframes = vl_frame2oell(frames)
% VL_FRAMES2OELL   Convert a geometric frame to an oriented ellipse
%   EFRAME = VL_FRAME2OELL(FRAME) converts the generic FRAME to an
%   oriented ellipses EFRAME. FRAME and EFRAME can be matrices, with
%   one frame per column.
%
%   A frame is either a point, a disc, an oriented disc, an ellipse,
%   or an oriented ellipse. These are represented respectively by 2,
%   3, 4, 5 and 6 parameters each, as described in VL_PLOTFRAME().  An
%   oriented ellipse is the most general geometric frame; hence, there
%   is no loss of information in this conversion.
%
%   If FRAME is an oriented disc or ellipse, then the conversion is
%   immediate. If, however, FRAME is not oriented (it is either a
%   point or an unoriented disc or ellipse), then an orientation must
%   be assigned. The orientation is chosen in such a way that the
%   affine transformation that maps the standard oriented frame into
%   the output EFRAME does not rotate the Y axis. If frames represent
%   detected visual features, this convention corresponds to assume
%   that features are upright.
%
%   If FRAME is a point, then the output is an ellipse with null area.
%
%   See: <a href="matlab:vl_help('tut.frame')">feature frames</a>,
%   VL_PLOTFRAME(), VL_HELP().

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

[D,K] = size(frames) ;
eframes = zeros(6,K) ;

switch D
  case 2
    eframes(1:2,:) = frames(1:2,:) ;

  case 3
    eframes(1:2,:) = frames(1:2,:) ;
    eframes(3,:)   = frames(3,:) ;
    eframes(6,:)   = frames(3,:) ;

  case 4
    r = frames(3,:) ;
    c = r.*cos(frames(4,:)) ;
    s = r.*sin(frames(4,:)) ;

    eframes(1:2,:) = frames(1:2,:) ;
    eframes(3:6,:) = [c ; s ; -s ; c] ;

  case 5
    eframes(1:2,:) = frames(1:2,:) ;
    eframes(3:6,:) = mapFromS(frames(3:5,:)) ;

  case 6
    eframes = frames ;

  otherwise
     error('FRAMES format is unknown.') ;
end

% --------------------------------------------------------------------
function A = mapFromS(S)
% --------------------------------------------------------------------
% Returns the (stacking of the) 2x2 matrix A that maps the unit circle
% into the ellipses satisfying the equation x' inv(S) x = 1. Here S
% is a stacked covariance matrix, with elements S11, S12 and S22.
%
% The goal is to find A such that AA' = S. In order to let the Y
% direction unaffected (upright feature), the assumption is taht
% A = [a b ; 0 c]. Hence
%
%  AA' = [a^2, ab ; ab, b^2+c^2] = S.

A = zeros(4,size(S,2)) ;
a = sqrt(S(1,:));
b = S(2,:) ./ max(a, 1e-18) ;

A(1,:) = a ;
A(2,:) = b ;
A(4,:) = sqrt(max(S(3,:) - b.*b, 0)) ;
