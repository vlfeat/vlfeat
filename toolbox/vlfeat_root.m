function path = vlfeat_root
% VLFEAT_ROOT  Obtain VLFeat package root directory
%   PATH = VLFEAT_ROOT() returns the root directory of the VLFeat
%   package.
%   
%   See also:: VLFEAT_SETUP(), HELP_VLFEAT().

[a,b,c] = fileparts(which('vlfeat_root')) ;
[a,b,c] = fileparts(a) ;
path = a;
