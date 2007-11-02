% DEMO_SIFT_CMD Demo: SIFT: test command line utility

p_img = fullfile(vlfeat_root,'data','box.pgm') ;
p_low = fullfile(vlfeat_root,'data','box.sift') ;
p_txt = fullfile(vlfeat_root,'results','box.sift') ;
p_tmp = fullfile(vlfeat_root,'results','box.tmp') ;

I = imread(p_img) ;
I = single(I) ;

[f0,d0] = ubcread(p_low) ;

% --------------------------------------------------------------------
%                                                        Test detector
% --------------------------------------------------------------------

[err,msg] = demo_cmd('sift',[p_img ' --output=ascii://' p_txt ...
                   ' --peak-thresh=3.4 --edge-thresh=10']) ;
if (err), error(msg) ; end

data      = load(p_txt,'-ASCII') ;
f1        = data(:,1:4)' ; f1(1:2,:) = f1(1:2,:) + 1 ;
d1        = uint8(data(:,5:end)') ;
[f,d]     = sift(I,'FirstOctave',-1,'edgethresh',10,'peakthresh',3.4) ;

disp(msg) ;

[m0, sc0]  = ubcmatch(d,d0) ;
[m1, sc1]  = ubcmatch(d,d1) ;
[dr,perm]  = sort(sc0,'ascend') ;

figure(1) ; clf ;
imagesc(I) ; colormap gray ; hold on ;
h = plotframe(f) ;  set(h,'color','g','linewidth',4) ;
h = plotframe(f1) ; set(h,'color','b','linewidth',2) ;
h = plotframe(f0) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
demo_print('sift_cmd_1') ;

figure(2) ; clf ;
imagesc(I) ; colormap gray ; hold on ;
s      = m0(1,perm(1:20)) ;
s0     = m0(2,perm(1:20)) ;
[dr,a] = intersect(m1(1,:),s) ;
s1     = m1(2,a) ;

h = plotsiftdescriptor(d(:,s),  f(:,s)  ) ; set(h,'color','g','linewidth',4) ;
h = plotsiftdescriptor(d1(:,s1),f1(:,s1)) ; set(h,'color','b','linewidth',2) ;
h = plotsiftdescriptor(d0(:,s0),f0(:,s0)) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
demo_print('sift_cmd_2') ;

% --------------------------------------------------------------------
%                                                      Test descriptor
% --------------------------------------------------------------------

tmp=f0' ; tmp(:,1:2) = tmp(:,1:2) - 1 ;
save(p_tmp,'tmp','-ASCII') ;

[err,msg] = demo_cmd('sift',[p_img ...
                    ' --output=ascii://' p_txt ...
                    ' --read-frames=ascii://' p_tmp ...
                    ' -v']) ;
data      = load(p_txt,'-ASCII') ;
f1        = data(:,1:4)' ; f1(1:2,:) = f1(1:2,:) + 1 ;
d1        = uint8(data(:,5:end)') ;
[f,d]     = sift(I,'FirstOctave',-1,'frames',f0,'verbose') ;

disp(msg) ;

[m0, sc0]  = ubcmatch(d,d0) ;
[m1, sc1]  = ubcmatch(d,d1) ;
[dr,perm]  = sort(sc0,'ascend') ;

figure(3) ; clf ;
imagesc(I) ; colormap gray ; hold on ;
h = plotframe(f) ;  set(h,'color','g','linewidth',4) ;
h = plotframe(f1) ; set(h,'color','b','linewidth',2) ;
h = plotframe(f0) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
demo_print('sift_cmd_3') ;

figure(4) ; clf ;
imagesc(I) ; colormap gray ; hold on ;
s      = m0(1,perm(1:20)) ;
s0     = m0(2,perm(1:20)) ;
[dr,a] = intersect(m1(1,:),s) ;
s1     = m1(2,a) ;

h = plotsiftdescriptor(d(:,s),  f(:,s)  ) ; set(h,'color','g','linewidth',4) ;
h = plotsiftdescriptor(d1(:,s1),f1(:,s1)) ; set(h,'color','b','linewidth',2) ;
h = plotsiftdescriptor(d0(:,s0),f0(:,s0)) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
demo_print('sift_cmd_4') ;
