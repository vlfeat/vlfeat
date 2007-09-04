function [err,msg] = demo_cmd(name, args)
% DEMO_CMD: Demo: Command line utility

ext = '' ;
switch mexext
  case 'mexmac'
    arch = 'mac' ;
  case 'mexmaci'
    arch = 'maci' ;
  case 'mexglx'
    arch = 'glx' ;
  case 'dll' 
    arch = 'win32' ;
    ext = '.exe' ;
end

cmd = fullfile(vlfeat_root,'bin',arch,[name ext]) ;
str = [cmd ' ' args] ;
disp(str);
[err,msg] = system(str) ;
