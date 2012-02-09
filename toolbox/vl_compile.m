function vl_compile(compiler)
% VL_COMPILE  Compile VLFeat MEX files
%   VL_COMPILE() uses MEX() to compile VLFeat MEX files. This command
%   works only under Windows and is used to re-build problematic
%   binaries. The preferred method of compiling VLFeat on both UNIX
%   and Windows is through the provided Makefiles.
%
%   VL_COMPILE() only compiles the MEX files and assumes that the
%   VLFeat DLL (i.e. the file VLFEATROOT/bin/win{32,64}/vl.dll) has
%   already been built. This file is built by the Makefiles.
%
%   By default VL_COMPILE() assumes that Visual C++ is the active
%   MATLAB compiler. VL_COMPILE('lcc') assumes that the active
%   compiler is LCC instead (see MEX -SETUP). Unfortunately LCC does
%   not seem to be able to compile the latest versions of VLFeat due
%   to bugs in the support of 64-bit integers. Therefore it is
%   recommended to use Visual C++ instead.
%
%   See also: VL_NOPREFIX(), VL_HELP().

% Authors: Andrea Vedadli, Jonghyun Choi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if nargin < 1, compiler = 'visualc' ; end
switch lower(compiler)
  case 'visualc'
    fprintf('%s: assuming that Visual C++ is the active compiler\n', mfilename) ;
    useLcc = false ;
  case 'lcc'
    fprintf('%s: assuming that LCC is the active compiler\n', mfilename) ;
    warning('LCC may fail to compile VLFeat. See help vl_compile.') ;
    useLcc = true ;
  otherwise
    error('Unknown compiler ''%s''.', compiler)
end

vlDir = vl_root ;
toolboxDir = fullfile(vlDir, 'toolbox') ;

switch computer
  case 'PCWIN'
    fprintf('%s: compiling for PCWIN (32 bit)\n', mfilename);
    mexwDir = fullfile(toolboxDir, 'mex', 'mexw32') ;
    binwDir = fullfile(vlDir, 'bin', 'win32') ;
  case 'PCWIN64'
    fprintf('%s: compiling for PCWIN64 (64 bit)\n', mfilename);
    mexwDir = fullfile(toolboxDir, 'mex', 'mexw64') ;
    binwDir = fullfile(vlDir, 'bin', 'win64') ;
  otherwise
    error('The architecture is neither PCWIN nor PCWIN64. See help vl_compile.') ;
end

impLibPath = fullfile(binwDir, 'vl.lib') ;
libDir = fullfile(binwDir, 'vl.dll') ;

mkd(mexwDir) ;

% find the subdirectories of toolbox that we should process
subDirs = dir(toolboxDir) ;
subDirs = subDirs([subDirs.isdir]) ;
discard = regexp({subDirs.name}, '^(.|..|noprefix|mex.*)$', 'start') ;
keep    = cellfun('isempty', discard) ;
subDirs = subDirs(keep) ;
subDirs = {subDirs.name} ;

% Copy support files  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if ~exist(fullfile(binwDir, 'vl.dll'))
  error('The VLFeat DLL (%s) could not be found. See help vl_compile.', ...
    fullfile(binwDir, 'vl.dll')) ;
end
tmp = dir(fullfile(binwDir, '*.dll')) ;
supportFileNames = {tmp.name} ;
for fi = 1:length(supportFileNames)
  name = supportFileNames{fi} ;
  cp(fullfile(binwDir, name),  ...
     fullfile(mexwDir, name)   ) ;
end

% Ensure implib for LCC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if useLcc
  lccImpLibDir  = fullfile(mexwDir, 'lcc') ;
  lccImpLibPath = fullfile(lccImpLibDir, 'VL.lib') ;
  lccRoot       = fullfile(matlabroot, 'sys', 'lcc', 'bin') ;
  lccImpExePath = fullfile(lccRoot, 'lcc_implib.exe') ;

  mkd(lccImpLibDir) ;
  cp(fullfile(binwDir, 'vl.dll'), fullfile(lccImpLibDir, 'vl.dll')) ;

  cmd = ['"' lccImpExePath '"', ' -u ', '"' fullfile(lccImpLibDir, 'vl.dll') '"'] ;
  fprintf('Running:\n> %s\n', cmd) ;

  curPath = pwd ;
  try
    cd(lccImpLibDir) ;
    [d,w] = system(cmd) ;
    if d, error(w); end
    cd(curPath) ;
  catch
    cd(curPath) ;
    error(lasterr) ;
  end
end

% Compile each mex file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
for i = 1:length(subDirs)
  thisDir = fullfile(toolboxDir, subDirs{i}) ;
  fileNames = ls(fullfile(thisDir, '*.c'));

  for f = 1:size(fileNames,1)
    fileName = fileNames(f, :) ;

    sp  = strfind(fileName, ' ');
    if length(sp) > 0, fileName = fileName(1:sp-1); end

    filePath = fullfile(thisDir, fileName);
    fprintf('MEX %s\n', filePath);

    dot = strfind(fileName, '.');
    mexFile = fullfile(mexwDir, [fileName(1:dot) 'dll']);
    if exist(mexFile)
      delete(mexFile)
    end

    cmd = {['-I' toolboxDir],   ...
           ['-I' vlDir],        ...
           '-O',                ...
          '-outdir', mexwDir, ...
           filePath             } ;

    if useLcc
      cmd{end+1} = lccImpLibPath ;
    else
      cmd{end+1} = impLibPath ;
    end
    mex(cmd{:}) ;
  end
end

% --------------------------------------------------------------------
function cp(src,dst)
% --------------------------------------------------------------------
if ~exist(dst,'file')
  fprintf('Copying ''%s'' to ''%s''.\n', src,dst) ;
  copyfile(src,dst) ;
end

% --------------------------------------------------------------------
function mkd(dst)
% --------------------------------------------------------------------
if ~exist(dst, 'dir')
  fprintf('Creating directory ''%s''.', dst) ;
  mkdir(dst) ;
end
