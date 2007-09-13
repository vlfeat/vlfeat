function I = imreadgray(file,varargin) 
% IMREADGRAY  Reads an image as gray-scale
%   I=IMREADGRAY(FILE) reads the image from file FILE and converts the
%   result to a gray scale image (DOUBLE storage class ranging in
%   [0,1]).
%
%   IMREADGRAY(FILE,FMT) specifies the file format FMT (see IMREAD()).
%
%   See RGB2DOUBLE().

% AUTORIGHTS

I = imread(file) ;
I = im2double(I) ;

if(size(I,3) > 1)
  I = rgb2gray(I) ;
end
