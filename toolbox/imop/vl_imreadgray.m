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
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

I = imread(file) ;
I = im2double(I) ;

if(size(I,3) > 1)
  I = rgb2gray(I) ;
end
