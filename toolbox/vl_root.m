function path = vl_root
% VL_ROOT  Obtain VLFeat root path
%   PATH = VL_ROOT() returns the path to the VLFeat installation.
%   
%   See also:: VL_SETUP(), VL_VLFEAT().
%   Authors:: Andrea Vedaldi and Brian Fulkerson

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

[a,b,c] = fileparts(mfilename('fullpath')) ;
[a,b,c] = fileparts(a) ;
path = a ;
