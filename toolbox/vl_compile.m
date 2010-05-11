function vl_compile(useLcc)
% VL_COMPILE  Compile MEX files
%   VL_COMPILE() uses MEX() to compile VLFeat MEX files. This command
%   is needed moslty under Windows to re-build problematic binares.
%
%   By default VL_COMPILE() assumes that the MATLAB bundled compiler LCC
%   is the active compiler. Use VL_COMPILE(FALSE) if Visual C++
%   is the active compiler (see MEX -SETUP).
%
%   See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if nargin < 1
  warning('Assuming the LCC compiler. Please use vl_compile(false) if you are using the Visual C compiler') ;
  useLcc     = true ;
end

vlDir      = vl_root ;
toolboxDir = fullfile(vlDir, 'toolbox') ;
mexw32Dir  = fullfile(toolboxDir, 'mexw32') ;
binw32Dir  = fullfile(vlDir, 'bin', 'w32') ;
impLibPath = fullfile(binw32Dir, 'vl.lib') ;
libDir     = fullfile(binw32Dir, 'vl.dll') ;

mkd(mexw32Dir) ;

% find the subdirectories of toolbox that we should process
subDirs = dir(toolboxDir) ;
subDirs = subDirs([subDirs.isdir]) ;
discard = regexp({subDirs.name}, '^(.|..|noprefix|mex.*)$', 'start') ;
keep    = cellfun('isempty', discard) ;
subDirs = subDirs(keep) ;
subDirs = {subDirs.name} ;

% Copy support files  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
tmp = dir(fullfile(binw32Dir, '*.dll')) ;
supportFileNames = {tmp.name} ;
for fi = 1:length(supportFileNames)
  name = supportFileNames{fi} ;
  cp(fullfile(binw32Dir, name),  ...
     fullfile(mexw32Dir, name)   ) ;
end

% Ensure implib for LCC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if useLcc
  lccImpLibDir  = fullfile(mexw32Dir, 'lcc') ;
  lccImpLibPath = fullfile(lccImpLibDir, 'VL.lib') ;
  lccRoot       = fullfile(matlabroot, 'sys', 'lcc', 'bin') ;
  lccImpExePath = fullfile(lccRoot, 'lcc_implib.exe') ;
  
  mkd(lccImpLibDir) ;
  cp(fullfile(binw32Dir, 'vl.dll'), fullfile(lccImpLibDir, 'vl.dll')) ;

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
    mexFile = fullfile(mexw32Dir, [fileName(1:dot) 'dll']);
    if exist(mexFile)
      delete(mexFile)
    end
    
    cmd = {['-I' toolboxDir],   ...
           ['-I' vlDir],        ...
           '-O',                ...
          '-outdir', mexw32Dir, ...
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

