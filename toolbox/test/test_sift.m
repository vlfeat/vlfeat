function test_sift
% TEST_SIFT  Test SIFT implementation(s)

I = test_pattern(101);

% run various instances of the code
[a0,b0]  = sift(single(I),'verbose','peaktresh',0,'levels',4) ;
[a1,b1]  = cmd_sift(I,'--first-octave=0 --peak-tresh=0 --levels=4') ;
[a2,b2]  = cmd_sift(I,'--first-octave=0',1) ;
[a3,b3]  = sift(single(I),'verbose','frames',a0) ;

figure(1) ; clf ; colormap gray ; imagesc(I) ; hold on ;
h=plotsiftdescriptor(b0,a0) ; set(h,'color','g','linewidth',6) ;
h=plotsiftdescriptor(b1,a1) ; set(h,'color','b','linewidth',4) ;
h=plotsiftdescriptor(b2,a2) ; set(h,'color','r','linewidth',2) ;
h=plotsiftdescriptor(b3,a3) ; set(h,'color','y','linewidth',1) ;
title('Same descriptor computed in four ways') ;

%disp([a0 a1 a2 a3]) ;

% --------------------------------------------------------------------
function [a,b]=cmd_sift(I,param,do_read)
% --------------------------------------------------------------------

switch mexext
  case 'mexmac'
    arch = 'mac/sift' ;
  case 'mexmaci'
    arch = 'maci/sift' ;
  case 'mexglx'
    arch = 'glx/sift' ;
  case 'dll' ;
    arch = 'win32\sift.exe' ;
end

pfx_sift_cmd = fullfile(vlfeat_root,'bin',arch) ;
pfx_im       = fullfile(vlfeat_root,'results','autotest.pgm') ;
pfx_d        = fullfile(vlfeat_root,'results','autotest.descr') ;
pfx_f        = fullfile(vlfeat_root,'results','autotest.frame') ;

imwrite(uint8(I), pfx_im) ;

str = [pfx_sift_cmd, ' ', param, ' ', ...
       ' --descriptors=', pfx_d, ...,
       ' --frames=',      pfx_f, ...,
       ' -v -v ' pfx_im] ;

if (nargin > 2)
  str = [str ' --read-frames=' pfx_f] ;
end

fprintf('> %s\n',str) ;

[err,msg] = system(str) ;

if (err), error(msg) ; end

fprintf(msg) ;

a = load(pfx_f,'-ASCII')' ;
b = load(pfx_d,'-ASCII')' ;

if ~isempty(a), a(1:2,:) = a(1:2,:) + 1 ; end
