% IMINTEGRAL  Integral image
%   J = IMINTEGRAL(I) calculates the integral image I of the image I.
%   I must a matrix with DOUBLE storage class. J is given by
%
%    J(i,j) = sum(I(1:i,1:j))
%
%   and has one column and one row more than I.
%
%   Example:
%     IMINTEGRAL(ONES(3)) = [ 1 2 3 ; 
%                             2 4 6 ; 
%                             3 6 9 ]
% 
%   See also HELP_VLFEAT().

% AUTORIGHTS
%
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.
