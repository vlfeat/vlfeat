function vl_xmkdir(path, varargin)
% VL_XMKDIR  Create a directory recursively.
%   VL_XMKDIR(PATH) creates all directory specified by PATH if they
%   do not exist (existing directories are skipped).
%
%   The function is meant as a silent replacement of MATLAB's builtin
%   MKDIR() function. It can also be used to show more clearly what
%   directories are or would be created by the command.
%
%   The function accepts the following options:
%
%   Pretend:: false
%     If TRUE the function does not create any directoty (implies
%     'Verbose').
%
%   Verbose:: false
%     If TRUE the function prints the operations performed.
%
%   See also: VL_HELP().

% Authors: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.pretend = false ;
opts.verbose = false ;
opts = vl_argparse(opts,varargin) ;

opts.verbose = opts.verbose | opts.pretend ;

[subPath, name, ext] = fileparts(path) ;
name = [name ext] ;

% Paths are parsed as sequences of blocks
%
% ('/', 'DIR1/', 'DIR2/', ... , 'DIRK/') + DIRNAME
%
% So for example:
%
% ''    -> () + ''
% a     -> () + 'a'
% a/    -> ('a/') + ''
% a/b   -> ('a/') + 'b'
% a/b/  -> ('a/', 'b/') + ''
% /     -> ('/') + ''
% /a    -> ('/') + 'a'
% /a/   -> ('/', 'a/') + ''
% /a/b  -> ('/', 'a/') + 'b'
% /a/b/ -> ('/, 'a/', 'b/) + ''
%
% DIRNAME is empty if PATH ends with / and is correctly extracted in
% this way by FILEPARTS(). SUBDIR is always different from PATH except
% for the base cases PATH='/' or PATH='', for which the recursion
% stops.

if ~strcmp(subPath, path)
  vl_xmkdir(subPath, varargin{:}) ;
end

% name is empty if path = '*/'
if isempty(name)
  return ;
end

if opts.verbose
  fprintf('%s: directory ''%s'' in ''%s''', mfilename, name, subPath) ;
end

if ~exist(path, 'dir')
  if opts.pretend && opts.verbose
    fprintf(' does not exist, would create.\n') ;
  else
    [success,message,messageId] = mkdir(fullfile(subPath, name)) ;
    if opts.verbose
      fprintf(' did not exist, created.\n') ;
    end
  end
else
  if opts.verbose
    if opts.pretend
      fprintf(' exists, would not create.\n') ;
    else
      fprintf(' exists, not creating.\n') ;
    end
  end
end
