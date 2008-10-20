function path = vl_setup
% VL_SETUP Add VLFeat toolbox path to MATLAB path
%  PATH = VL_SETUP() adds VLFeat to MATLAB path.
%
%  See also:: VL_HELP().

bindir = mexext ;
if strcmp(bindir, 'dll'), bindir = 'mexw32' ; end

root = vl_root ;
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
addpath(fullfile(root,'toolbox',bindir    )) ;

fprintf('Welcome to VLFeat!\n') ;
