function I = vl_imreadgray(file,varargin)
% VL_IMREADGRAY  Reads an image as gray-scale
%   I=VL_IMREADGRAY(FILE) reads the image from file FILE and converts the
%   result to a gray scale image (DOUBLE storage class ranging in
%   [0,1]).
%
%   VL_IMREADGRAY(FILE,FMT) specifies the file format FMT (see IMREAD()).
%
%   See also: RGB2DOUBLE(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

I = imread(file) ;
I = im2double(I) ;

if(size(I,3) > 1)
  I = rgb2gray(I) ;
end
