basedir = fullfile(vlfeat_root, 'toolbox');
dirlist = {'aib', 'demo', 'geometry', 'imop', 'kmeans', 'misc', 'mser', ...
           'plotop', 'sift', 'special', 'test'};

outdir = fullfile(basedir, 'mexw32');
if ~exist(outdir) mkdir(outdir); end

for i = 1:length(dirlist)
  files = ls(fullfile(basedir, dirlist{i}, '*.c'));
  for f = 1:size(files,1)
    file = files(f,:);
    sp = strfind(files(f,:), ' ');
    if length(sp) > 0, file = file(1:sp-1); end
    fprintf('MEX %s\n', file);
    filename = fullfile(basedir, dirlist{i}, file);
    cmd = {filename, ['-I' basedir], ['-I' basedir '\..\'], '-O', ...
          '-outdir', outdir, [basedir '\..\bin\win32\vl.lib']};
    mex(cmd{:});
  end
end


if ~exist(fullfile(outdir, 'vl.dll'))
  fprintf('Copying support files to %s\n', outdir);
  copyfile([basedir '\..\bin\win32\vl.dll'], outdir); 
  copyfile([basedir '\..\bin\win32\msvcr*.dll'], outdir); 
end
