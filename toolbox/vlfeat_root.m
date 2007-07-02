function path = vlfeat_root
% VLFEAT_ROOT  Get VlFeat package root directory
%  PATH = VLFEATROOT() returns the root directory of the VlFeat
%  package.

[a,b,c] = fileparts(which('vlfeat_root')) ;
[a,b,c] = fileparts(a) ;
path = a;
