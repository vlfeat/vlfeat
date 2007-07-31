% BINSUM  Binned sum
%  H=BINSUM(H,X,B) adds the elements of X to the elements of H
%  indicated by B. X and B are arrays of the same dimensions, and the
%  elements of B must be valid indexes for the array H.
%
%  H=BINSUM(H,X,B,DIM) operates only on the specified dimension
%  DIM. In this case, H, X and B are array of the same dimensions,
%  except for the dimension DIM of H which may differ. The indexes B
%  are subscript for the dimension DIM of H.
%
%  This function is very useful to compute histograms.
%
%  Examples: BINSUM([0 0],  1, 2) = [0 1] ;
%            BINSUM([1 7], -1, 1) = [0 7] ;
%            BINSUM([0 0; 0 0], [1 1], [1 2], 1) = [1 0; 0 1] ;

% AUTORIGHTS
% Copyright (C) 2006 Andrea Vedaldi
%       
% This file is part of VLUtil.
% 
% VLUtil is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software Foundation,
% Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
