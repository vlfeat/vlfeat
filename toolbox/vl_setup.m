function path = vl_setup(varargin)
% VL_SETUP Add VLFeat Toolbox to the path
%   PATH = VL_SETUP() adds the VLFeat Toolbox to MATLAB path and
%   returns the path PATH to the VLFeat package.
%
%   VL_SETUP('NOPREFIX') adds aliases to each function that do not
%   contain the VL_ prefix. For example, with this option it is
%   possible to use SIFT() instead of VL_SIFT().
%
%   VL_SETUP('TEST') or VL_SETUP('XTEST') adds VLFeat unit test
%   function suite. See also VL_TEST().
%
%   VL_SETUP('QUIET') does not print the greeting message.
%
%   See also:: VL_HELP(), VL_ROOT().
%   Authors:: Andrea Vedaldi and Brian Fulkerson

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

noprefix = false ;
quiet = false ;
xtest = false ;
demo = false ;

for ai=1:length(varargin)
  opt = varargin{ai} ;
  switch lower(opt)
    case {'noprefix', 'usingvl'}
      noprefix = true ;
    case {'test', 'xtest'}
      xtest = true ;
    case {'demo'}
      demo = true ;
    case {'quiet'}
      quiet = true ;
    otherwise
      error('Unknown option ''%s''.', opt) ;
  end
end

if exist('octave_config_info')
  bindir = 'octave' ;
else
  bindir = mexext ;
  if strcmp(bindir, 'dll'), bindir = 'mexw32' ; end
end
bindir = fullfile('mex',bindir) ;

[a,b,c] = fileparts(mfilename('fullpath')) ;
[a,b,c] = fileparts(a) ;
path = a ;

root = vl_root ;
addpath(fullfile(root,'toolbox'             )) ;
addpath(fullfile(root,'toolbox','aib'       )) ;
addpath(fullfile(root,'toolbox','geometry'  )) ;
addpath(fullfile(root,'toolbox','imop'      )) ;
addpath(fullfile(root,'toolbox','kmeans'    )) ;
addpath(fullfile(root,'toolbox','misc'      )) ;
addpath(fullfile(root,'toolbox','mser'      )) ;
addpath(fullfile(root,'toolbox','plotop'    )) ;
addpath(fullfile(root,'toolbox','quickshift')) ;
addpath(fullfile(root,'toolbox','sift'      )) ;
addpath(fullfile(root,'toolbox','special'   )) ;
addpath(fullfile(root,'toolbox',bindir      )) ;

if noprefix
  addpath(fullfile(root,'toolbox','noprefix')) ;
end

if xtest
  addpath(fullfile(root,'toolbox','xtest')) ;
end

if demo
  addpath(fullfile(root,'toolbox','demo')) ;
end

if ~quiet
  fprintf('VLFeat %s ready.\n', vl_version) ;
end

if nargout == 0
  clear path ;
end
