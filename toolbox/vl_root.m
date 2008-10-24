function path = vl_root
% VL_ROOT  Obtain VLFeat root path
%   PATH = VL_ROOT() returns the path to the VLFeat installation.
%   
%   See also:: VL_SETUP(), VL_VLFEAT().
%   Authors:: Andrea Vedaldi and Brian Fulkerson

% AUTORIGHTS

[a,b,c] = fileparts(mfilename('fullpath')) ;
[a,b,c] = fileparts(a) ;
path = a ;
