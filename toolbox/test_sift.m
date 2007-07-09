function test_sift
% TEST_SIFT  Test SIFT implementation(s)

%I = imreadbw('../data/box.pgm') ;
%I = single(I*255) ;

% generate test pattern
ur    = linspace(-1,1,128) ;
vr    = linspace(-1,1,128) ;
[u,v] = meshgrid(ur,vr);
%I     = u.^2 + v.^2 > (1/4).^2 ;
I     = abs(u) + abs(v) > (1/4) ;
I     = 255 * I ;
I(1:64,:) = 0 ;

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

disp([a0 a1 a2 a3]) ;

% --------------------------------------------------------------------
function [a,b]=cmd_sift(I,param,do_read)
% --------------------------------------------------------------------

root   = vlfeat_root ;
pfx_im = fullfile(root,'results','autotest.pgm') ;

imwrite(I, pfx_im) ;

switch mexext
  case 'mexmac'
    arch = 'mac' ;
  case 'mexmaci'
    arch = 'maci' ;
  case 'mexglx'
    arch = 'glx' ;
end

pfx_sift_cmd = fullfile(vlfeat_root,'bin',arch,'sift') ;
pfx_d        = fullfile(vlfeat_root,'results','autotest.descr') ;
pfx_f        = fullfile(vlfeat_root,'results','autotest.frames') ;

str = [pfx_sift_cmd, ' ', param, ' ', ...
       ' --descriptors=', pfx_d, ...
       ' --frames=',      pfx_f, ...
       ' -v -v ' pfx_im] ;

if (nargin > 2)
  str = [str ' --read-frames=' pfx_f] ;
end

fprintf('> %s',str) ;

[err,msg] = system(str) ;

if (err)
  error(msg)
end

disp(msg) ;

a = load(pfx_f,'-ASCII')' ;
b = load(pfx_d,'-ASCII')' ;

if ~isempty(a), a(1:2,:) = a(1:2,:) + 1 ;end