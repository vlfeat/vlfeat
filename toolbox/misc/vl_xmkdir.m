function vl_xmkdir(path, varargin)
% VL_XMKDIR  Create a directory recursively.
%   VL_XMKDIR(PATH) creates all directory specified by PATH if they
%   do not exist (existing directories are skipped).
%
%   The function accepts the following options:
%
%   Pretend:: [false]
%     Set to true to avoid creating any directory but print which
%     directories would be created (implies 'Verbose',true).
%
%   Verbose:: [false]
%     Set to true to print the operations performed.
%
%   See also: VL_HELP().

% Authors: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

pretend = false ;
verbose = false ;

for ai=1:2:length(varargin)
  opt = varargin{ai} ;
  arg = varargin{ai+1} ;
  switch lower(opt)
    case 'pretend'
      pretend = arg ;
    case 'verbose'
      verbose = arg ;
    otherwise
      error('Unknown option ''%s''.', opt) ;
  end
end

verbose = verbose | pretend ;

if isempty(path)
  return ;
end

[subPath, name, ext] = fileparts(path) ;
name = [name ext] ;

% subPath is equal to path if path = '/'
if ~strcmp(subPath, path)
  vl_xmkdir(subPath, varargin{:}) ;
end

% name is empty if path = '*/'
if isempty(name)
  return ;
end

if verbose
  fprintf('%s: directory ''%s'' in ''%s''', mfilename, name, subPath) ;
end

if ~exist(path, 'dir')
  if pretend && verbose
    fprintf(' does not exist, would create.\n') ;
  else
    mkdir(subPath, name) ;
    if verbose
      fprintf(' did not exist, created.\n') ;
    end
  end
else
  if verbose
    if pretend
      fprintf(' exists, would not create.\n') ;
    else
      fprintf(' exists, not creating.\n') ;
    end
  end
end
