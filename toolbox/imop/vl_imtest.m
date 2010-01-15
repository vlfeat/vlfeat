function I = vl_imtest(type, varargin)
% VL_IMTEST

switch lower(type)
  case 'box'
    I = imread(fullfile(vl_root, 'data', 'box.pgm')) ;
end
