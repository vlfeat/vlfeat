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
%   See also: VL_ROOT(), VL_HELP().

% Authors: Andrea Vedaldi and Brian Fulkerson

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

noprefix = false ;
quiet = true ;
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
    case {'verbose'}
      quiet = false ;
    otherwise
      error('Unknown option ''%s''.', opt) ;
  end
end

% Do not use vl_root() to avoid conflicts with other VLFeat
% installations.

[a,b,c] = fileparts(mfilename('fullpath')) ;
[a,b,c] = fileparts(a) ;
root = a ;

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
addpath(fullfile(root,'toolbox','slic'      )) ;
addpath(fullfile(root,'toolbox','gmm'       )) ;
addpath(fullfile(root,'toolbox','vlad'      )) ;
addpath(fullfile(root,'toolbox','fisher'    )) ;

if vl_isoctave()
  addpath(genpath(fullfile(root,'toolbox','mex','octave'))) ;
  warning('off', 'Octave:possible-matlab-short-circuit-operator') ;
  pkg load image ;
else
  bindir = mexext ;
  if strcmp(bindir, 'dll'), bindir = 'mexw32' ; end
  addpath(fullfile(root,'toolbox','mex',bindir)) ;
end

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
  if exist('vl_version') == 3
    fprintf('VLFeat %s ready.\n', vl_version) ;
  else
    warning('VLFeat does not seem to be installed correctly. Make sure that the MEX files are compiled.') ;
  end
end

if nargout == 0
  clear path ;
end
