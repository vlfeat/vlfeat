function vl_noprefix
% VL_NOPREFIX  Create a prefix-less version of VLFeat commands
%   VL_NOPREFIX() creats prefix-less stubs for VLFeat functions
%   (e.g. SIFT for VL_SIFT). This function is seldom used as the stubs
%   are included in the VLFeat binary distribution anyways. Moreover,
%   on UNIX platforms, the stubs are generally constructed by the
%   Makefile.
%
%   See also: VL_COMPILE(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

root = fileparts(which(mfilename)) ;
list = listMFilesX(root);
outDir = fullfile(root, 'noprefix') ;

if ~exist(outDir, 'dir')
  mkdir(outDir) ;
end

for li = 1:length(list)
  name = list(li).name(1:end-2) ; % remove .m
  nname = name(4:end) ; % remove vl_
  stubPath = fullfile(outDir, [nname '.m']) ;
  fout = fopen(stubPath, 'w') ;

  fprintf('Creating stub %s for %s\n', stubPath, nname) ;

  fprintf(fout, 'function varargout = %s(varargin)\n', nname) ;
  fprintf(fout, '%% %s Stub for %s\n', upper(nname), upper(name)) ;
  fprintf(fout, '[varargout{1:nargout}] = %s(varargin{:})\n', name) ;

  fclose(fout) ;
end

end

function list = listMFilesX(root)
list = struct('name', {}, 'path', {}) ;
files = dir(root) ;
for fi = 1:length(files)
  name = files(fi).name ;
  if files(fi).isdir
    if any(regexp(name, '^(\.|\.\.|noprefix)$'))
      continue ;
    else
      tmp = listMFilesX(fullfile(root, name)) ;
      list = [list, tmp] ;
    end
  end
  if any(regexp(name, '^vl_(demo|test).*m$'))
    continue ;
  elseif any(regexp(name, '^vl_(demo|setup|compile|help|root|noprefix)\.m$'))
    continue ;
  elseif any(regexp(name, '\.m$'))
    list(end+1) = struct(...
      'name', {name}, ...
      'path', {fullfile(root, name)}) ;
  end
end
end
