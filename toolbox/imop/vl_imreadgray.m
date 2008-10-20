function I = vl_imreadgray(file,varargin) 
% VL_IMREADGRAY  Reads an image as gray-scale
%   I=VL_IMREADGRAY(FILE) reads the image from file FILE and converts the
%   result to a gray scale image (DOUBLE storage class ranging in
%   [0,1]).
%
%   VL_IMREADGRAY(FILE,FMT) specifies the file format FMT (see IMREAD()).
%
%   See also:: RGB2DOUBLE().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

I = imread(file) ;
I = im2double(I) ;

if(size(I,3) > 1)
  I = rgb2gray(I) ;
end
