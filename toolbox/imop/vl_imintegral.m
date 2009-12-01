% VL_IMINTEGRAL  Integral image
%   J = VL_IMINTEGRAL(I) calculates the integral image J of the image I.
%   I must a matrix with DOUBLE storage class. J is given by
%
%    J(i,j) = sum(I(1:i,1:j)).
%
%   Notice that J has the same size as I (often one also adds a null
%   column and row at the begnning of the matrix).
%
%   Example::
%     The following identity holds:
%       VL_IMINTEGRAL(ONES(3)) = [ 1 2 3 ; 
%                               2 4 6 ; 
%                               3 6 9 ]
% 
%   See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.
