function path = vl_setup(varargin)
% VL_SETUP Add VLFeat Toolbox to the path
%   PATH = VL_SETUP() adds the VLFeat Toolbox to MATLAB path and
%   returns the path PATH to the VLFeat package.
%
%   VL_SETUP('USINGVL') adds to the path also the prefix-less version
%   of each function (so, for instance, it is possible to use SIFT()
%   instead of the more verbose VL_SIFT()).
%
%   VL_SETUP('QUIET') does not print the greeting message.
%
%   See also:: VL_HELP(), VL_ROOT().
%   Authors:: Andrea Vedaldi and Brian Fulkerson

% AUTORIGHTS

usingvl = false ;
quiet   = false ;

for ai=1:length(varargin)
  opt = varargin{ai} ;
  switch lower(opt)
    case 'usingvl'
      usingvl = true ;
    case 'quiet'
      quiet = true ;
  end
end

bindir = mexext ;
if strcmp(bindir, 'dll'), bindir = 'mexw32' ; end

[a,b,c] = fileparts(mfilename('fullpath')) ;
[a,b,c] = fileparts(a) ;
path = a ;

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

if usingvl
  addpath(fullfile(root,'toolbox','usingvl')) ;
end

fprintf('** Welcome to the VLFeat Toolbox **\n') ;

if nargout == 0
  clear path ;
end
