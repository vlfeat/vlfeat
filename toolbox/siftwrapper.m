function [f d] = siftwrapper(varargin)
global callnum

[f d] = sift(varargin{:});
save(sprintf('sift_%02d.mat', callnum), 'f', 'd');

callnum = callnum + 1;
