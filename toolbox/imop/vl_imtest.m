function I = vl_imtest(type, varargin)
% VL_IMTEST

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

switch lower(type)
  case 'box'
    I = imread(fullfile(vl_root, 'data', 'box.pgm')) ;
end
