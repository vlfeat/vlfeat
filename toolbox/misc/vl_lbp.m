% VL_LBP  Local Binary Patterns
%   F = VL_LBP(IM, CELLSIZE) computes the Local Binary Pattern (LBP)
%   features for image I.
%
%   IM is divided in cells of size CELLSIZE. F is a three-dimensional
%   array containing one histograms of quantized LBP features per
%   cell. The witdh of F is FLOOR(WIDTH/CELLSIZE), where WIDTH is the
%   width of the image. The same for the height. The third dimension
%   is 58.
%
%   See also: <a href="matlab:vl_help('lbp')">LBP</a>, VL_LBPFLIPLR(),
%   VL_HELP().

% Copyright (C) 2013 Andrea Vedaldi.
% Copyright (C) 2010-11 Andrea Vedaldi.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
