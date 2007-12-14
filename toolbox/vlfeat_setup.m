function path = vlfeat_setup
% VLFEAT_SETUP  Add VLFeat toolbox path to MATLAB path
%  PATH = VLFEAT_SETUP() adds VLFeat to MATLAB path.

root=vlfeat_root ;
addpath(fullfile(root,'toolbox'           )) ;
addpath(fullfile(root,'toolbox','aib'     )) ;
addpath(fullfile(root,'toolbox','geometry')) ;
addpath(fullfile(root,'toolbox','imop'    )) ;
addpath(fullfile(root,'toolbox','kmeans'  )) ;
addpath(fullfile(root,'toolbox','misc'    )) ;
addpath(fullfile(root,'toolbox','mser'    )) ;
addpath(fullfile(root,'toolbox','plotop'  )) ;
addpath(fullfile(root,'toolbox','sift'    )) ;
addpath(fullfile(root,'toolbox','special' )) ;
addpath(fullfile(root,'toolbox','test'    )) ;

fprintf('Welcome to VLFeat!\n') ;

