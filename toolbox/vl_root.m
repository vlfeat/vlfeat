function path = vl_root
% VL_ROOT  Obtain VLFeat package root directory
%   PATH = VL_ROOT() returns the root directory of the VLFeat
%   package.
%   
%   See also:: VL_SETUP(), VL_VLFEAT().

[a,b,c] = fileparts(which('vl_root')) ;
[a,b,c] = fileparts(a) ;
path = a;
