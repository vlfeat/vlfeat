% updateMakeFileBuildTestVS2017.m
% 
% part 1: update Makefile.mak
% this script queries your matlab/filesystem for information about
% your matlab and visual studio installation, in order to update the
% makefile that ships with vlfeat. it writes two new files to /vlfeat:
%  0. vlfeat/Makefile.mak.new
%  1. vlfeat/runMakeFile.bat (deleted after use)
% 
% part 2: build vlfeat
% runMakeFile.bat contains instructions that run the updated Makefile after
% it is built (using nmake). this is bundled into a bat instead of calling
% the shell directly from matlab because it needs to call a separate bat 
% (VsDevCmd.bat, which was VSVARS32.bat in VS < 2017) to make visual studio 
% environment variables callable. 
% 
% part 3: test vlfeat
% run vlfeats built in test suite
%
% peter cook 2019

if ~exist('vlfeat','dir')
    % if this file came from the vlfeat repo, this portion of the code
    % shouldnt technically be reachable unless your paths are messed up
    !git clone https://github.com/vlfeat/vlfeat vlfeat
    cd vlfeat
else
    !git pull
    % if you really messed something up
    % !git reset --hard origin/mater
end

% clean out existing binaries
% TODO: add a flag to skip binary cleanup?
if exist(fullfile(pwd,'bin'),'dir')
    rmdir('bin','s')
end

% clean out existing mexfiles
% TODO: add a flag to skip mexfiles cleanup?
if exist(fullfile(pwd,'toolbox\mex'),'dir')
    rmdir('toolbox\mex','s')
end

% get some data & rewrite the makefile
v = ver();
v = v(strcmp({v.Name},'MATLAB'));
VER = sprintf('0.%s',v.Version); %0.9.1 for r2016b
c = strsplit(v.Version,'.');
MATLABVER = sprintf('%s0%s00',c{1},c{2});
ARCH = 'win64';
DEBUG = 'no';

% look for visual studio
% visual studio
% version : year
%     8.0 : 2005
%     9.0 : 2008
%    10.0 : 2010
%    11.0 : 2012
%    12.0 : 2013
%    14.0 : 2015
%    15.0 : 2017
%    16.0 : 2019

verMap = containers.Map({'8.0',  '9.0',  '10.0', '11.0', '12.0', '14.0', '15.0', '16.0'},...
                        {'2005', '2008', '2010', '2012', '2013', '2015', '2017', '2019'});

% query the registry for visual studio environment variables
[~,d] = dos('REG QUERY "HKLM\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\Sxs\VS7"');
VisualStudioVersion = char(regexp(d,'\d+\.\d','match'));
VSINSTALLDIR = char(regexp(d,'C:\\[^\n]*','match'));
VCINSTALLDIR = fullfile(VSINSTALLDIR,'VC');
MSVCROOT = VCINSTALLDIR;
MSVSVERYEAR = verMap(VisualStudioVersion);

MATLABROOT = matlabroot;

% locate cl.exe
d = dir(fullfile(MSVCROOT,'**\x64\cl.exe'));
CC = fullfile(d(1).folder, d(1).name);

% locate link.exe
d = dir(fullfile(MSVCROOT,'**\x64\link.exe'));
LINK = fullfile(d(1).folder, d(1).name);

% locate visual c redistributable
d = dir(fullfile(MSVCROOT,'**\MSVC'));
d = d(~(strcmp({d.name},'.') | strcmp({d.name},'..')));
VCTOOLSINSTALLDIR = fullfile(d(end).folder,d(end).name);

VCToolsRedistDir = dir(fullfile(MSVCROOT,'**\vcredist_x64.exe'));
VCToolsRedistDir = VCToolsRedistDir.folder;
msvcPath = fullfile(VCToolsRedistDir,'x64');
MSVCR_PATH = dir(fullfile(msvcPath,'\\*.CRT'));
MSVSVER = char(regexp(MSVCR_PATH(1).name, '\d+', 'Match'));
MSVCR_PATH = fullfile(MSVCR_PATH.folder,MSVCR_PATH.name);
% get DLL version from folder children
d = dir(fullfile(MSVCR_PATH,'*.DLL'));
d = regexp({d.name},'(\d+)(?=\.dll)','match');
d = sort([d{:}]);
MSVCDLLVER = d{round(length(d)/2)};

% locate windows SDK
[~,d] = dos('REG QUERY "HKLM\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows"');
d = strsplit(d,char(10));
d = d(~cellfun(@isempty,d));
[~,WindowsSdkDir] = dos(sprintf('REG QUERY %c%s%c /v InstallationFolder',34,d{end},34));
WindowsSdkDir = char(regexp(WindowsSdkDir,'C:\\[^\n]*','match'));
WINSDKROOT = WindowsSdkDir;
[~,WINDOWSSDKVERSION] = dos(sprintf('REG QUERY %c%s%c /v ProductVersion',34,d{end},34));
WINDOWSSDKVERSION = strtrim(char(regexp(WINDOWSSDKVERSION,'(?<=REG_SZ)[^\n]*','match')));


MSVCROOT = regexprep(MSVCROOT,'\\','\\\\');
WindowsSdkDir = regexprep(WindowsSdkDir,'\\','\\\\');
VCTOOLSINSTALLDIR = regexprep(VCTOOLSINSTALLDIR,'\\','\\\\');
VCToolsRedistDir = regexprep(VCToolsRedistDir,'\\','\\\\');
CC = regexprep(CC,'\\','\\\\');
LINK = regexprep(LINK,'\\','\\\\');
MSVCR_PATH = regexprep(MSVCR_PATH,'\\','\\\\');
WINSDKROOT = regexprep(WINSDKROOT,'\\','\\\\');
MATLABROOT = regexprep(MATLABROOT,'\\','\\\\');

MSVCROOT = sprintf('%c%s%c',34,MSVCROOT,34);
WindowsSdkDir = sprintf('%c%s%c',34,WindowsSdkDir,34);
VCTOOLSINSTALLDIR = sprintf('%c%s%c',34,VCTOOLSINSTALLDIR,34);
VCToolsRedistDir = sprintf('%c%s%c',34,VCToolsRedistDir,34);
CC = sprintf('%c%s%c',34,CC,34);
LINK = sprintf('%c%s%c',34,LINK,34);
MSVCR_PATH = sprintf('%c%s%c',34,MSVCR_PATH,34);
WINSDKROOT = sprintf('%c%s%c',34,WINSDKROOT,34);
% MATLABROOT = sprintf('%c%s%c',34,MATLABROOT,34);

% write new makefile
fptr = fopen('Makefile.mak','r');
s = fread(fptr,inf,'uint8=>char');
fclose(fptr);
s = s';

% replace VER
s = regexprep(s, 'VER = [^\r\n]*', sprintf('VER = %s',VER));

% replace MSVSVER & insert MSVSVERYEAR & MSVCDLLVER
s = regexprep(s, 'MSVSVER =[^\r\n]*',...
    sprintf('MSVSVER = %s\r\nMSVSVERYEAR = %s\r\nMSVCDLLVER = %s',...
    MSVSVER,MSVSVERYEAR,MSVCDLLVER));

% replace MSVCROOT
% s = regexprep(s, 'MSVCROOT = [^\r\n]*', sprintf('MSVCROOT = %s',MSVCROOT));

% % replace WINSDKROOT and insert VCTOOLSINSTALLDIR & VCToolsRedistDir & WINDOWSSDKVERSION
% s = regexprep(s, 'WINSDKROOT = [^\r\n]*',...
%     sprintf('WINSDKROOT = %s\r\nVCTOOLSINSTALLDIR = %s\r\nVCToolsRedistDir = %s\r\nWINDOWSSDKVERSION = %s',...
%     WindowsSdkDir,VCTOOLSINSTALLDIR,VCToolsRedistDir,WINDOWSSDKVERSION));

% replace MATLABROOT
s = regexprep(s, 'MATLABROOT = [^\r\n]*', sprintf('MATLABROOT = %s',MATLABROOT));

% replace MATLABVER
s = regexprep(s, 'MATLABVER = [^\r\n]*', sprintf('MATLABVER = %s',MATLABVER));

% update MEXOPT
s = regexprep(s, 'MSVSYEAR', 'MSVSVERYEAR');

% update CC
%s = regexprep(s, 'CC = [^\r\n]*', sprintf('CC = %s',CC));
s = regexprep(s, 'CC = [^\r\n]*',...
    'CC = "\$\(VCTOOLSINSTALLDIR\)\\bin\\Hostx64\\x64\\cl\.exe"');

% update LINK
% s = regexprep(s, 'LINK = [^\r\n]*', sprintf('LINK = %s',LINK));
s = regexprep(s, 'LINK = [^\r\n]*',...
    'LINK = "\$\(VCTOOLSINSTALLDIR\)\\bin\\Hostx64\\x64\\link\.exe"');

% update MSVCR_PATH
% s = regexprep(s, 'MSVCR_PATH = [^\r\n]*', sprintf('MSVCR_PATH = %s',MSVCR_PATH));
s = regexprep(s, 'MSVCR_PATH = [^\r\n]*',...
    'MSVCR_PATH = \$\(VCToolsRedistDir\)\\redist\\x64\\Microsoft\.VC\$\(MSVSVER\)\.CRT');

% update LIBPATH
s = regexprep(s, '\(MSVCROOT\)\\lib\\amd64', '(VCTOOLSINSTALLDIR)\\lib\\x64');
% s = regexprep(s, '/LIBPATH:"\$\(WINSDKROOT\)\\lib\\x64"',...
%     sprintf('/LIBPATH%c"%s\\Lib\\\\%s\\\\ucrt\\\\x64" \\\\\r\n\t\t /LIBPATH%c"%s\\Lib\\\\%s\\\\um\\\\x64"',...
%     58, WINSDKROOT, WINDOWSSDKVERSION, 58, WINSDKROOT, 'WINDOWSSDKVERSION'));
s = regexprep(s, '/LIBPATH:"\$\(WINSDKROOT\)\\lib\\x64"',...
    '\/LIBPATH\:"\$\(WINSDKROOT\)\\Lib\\\$\(WINDOWSSDKVERSION\)\\ucrt\\x64" \\\r\n\t\t\/LIBPATH\:"\$\(WINSDKROOT\)\\Lib\\\$\(WINDOWSSDKVERSION\)\\um\\x64"');

% update DLL linker instructions
% remove manifests
s = regexprep(s,'\$\(bindir\)\\\$\(MSVCR\)\.manifest \: "\$\(MSVCR_PATH\)\\\$\(MSVCR\)\.manifest"\r\n\s*copy \$\(\*\*\) "\$\(\@\)"\r\n','');
s = regexprep(s,'\$\(mexdir\)\\\$\(MSVCR\)\.manifest \: "\$\(MSVCR_PATH\)\\\$\(MSVCR\)\.manifest"\r\n\s*copy \$\(\*\*\) "\$\(\@\)"\r\n','');

% update msvcr => msvcp(?)
s = regexprep(s, 'msvcr\$\(MSVSVER\)\.dll\:',...
    sprintf('msvcp%c%c%s%c%cdll%c',36,40,'MSVCDLLVER',41,46,58));
s = regexprep(s, '(?<=\$\(MSVCR_PATH\)\\)msvcr\$\(MSVSVER\)',...
    sprintf('msvcp%c%c%s%c',36,40,'MSVCDLLVER',41));

% open and write new makefile
fptr = fopen('Makefile.mak.new', 'w');
fprintf(fptr, '%s', s);
fclose(fptr);

% developer command prompt path
% this path may not be valid for visual studio installations <2017
devCmdPath = fullfile(VSINSTALLDIR,'Common7\Tools\VsDevCmd.bat');

fptr = fopen('runMakeFile.bat','w');
fprintf(fptr, 'CALL %c%s%c\r\n', 34, devCmdPath, 34);
fprintf(fptr, 'cd %s\r\n', pwd);
fprintf(fptr, 'nmake /f Makefile.mak.new\r\n');
fclose(fptr);

% run nmake on the updated makefile
dos('runMakeFile.bat','-echo');
delete 'runMakeFile.bat'

cd toolbox\xtest
addpath ..\mex\mexw64
vl_test();
cd ..\..
