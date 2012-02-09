function path = vl_root
% VL_ROOT  Obtain VLFeat root path
%   PATH = VL_ROOT() returns the path to the VLFeat installation.
%
%   See also: VL_SETUP(), VL_HELP().

% Authors: Andrea Vedaldi and Brian Fulkerson

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

[a,b,c] = fileparts(mfilename('fullpath')) ;
[a,b,c] = fileparts(a) ;
path = a ;
