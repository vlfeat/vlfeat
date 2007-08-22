function path = vlfeat_setup
% VLFEAT_SETUP  Add VLFeat toolbox path to MATLAB path
%  PATH = VLFEAT_SETUP() adds VLFeat to MATLAB path.

root=vlfeat_root ;
addpath(fullfile(root,'toolbox')) ;
addpath(fullfile(root,'toolbox','test')) ;
