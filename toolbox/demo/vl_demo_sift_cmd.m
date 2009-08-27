% VL_DEMO_SIFT_CMD Demo: SIFT: test command line utility

pfx = fullfile(vl_root,'results') ;
if ~ exist(pfx, 'dir')
  mkdir(pfx) ;
end

p_img = fullfile(vl_root,'data','box.pgm') ;
p_low = fullfile(vl_root,'data','box.sift') ;
p_txt = fullfile(pfx,'box.sift') ;
p_tmp = fullfile(pfx,'box.tmp') ;

I = imread(p_img) ;
I = single(I) ;

[f0,d0] = vl_ubcread(p_low) ;

% --------------------------------------------------------------------
%                                                        Test detector
% --------------------------------------------------------------------

[err,msg] = vl_demo_cmd('sift',['"' p_img '" --output=ascii://"' p_txt ...
                   '" --peak-thresh=3.4 --edge-thresh=10 --first-octave=-1']) ;
if (err), error(msg) ; end

data      = load(p_txt,'-ASCII') ;
f1        = data(:,1:4)' ; f1(1:2,:) = f1(1:2,:) + 1 ;
d1        = uint8(data(:,5:end)') ;
[f,d]     = vl_sift(I,'FirstOctave',-1,'edgethresh',10,'peakthresh',3.4) ;

disp(msg) ;

[m0, sc0]  = vl_ubcmatch(d,d0) ;
[m1, sc1]  = vl_ubcmatch(d,d1) ;
[dr,perm]  = sort(sc0,'ascend') ;

figure(1) ; clf ;
imagesc(I) ; colormap gray ; hold on ;
h = vl_plotframe(f) ;  set(h,'color','g','linewidth',4) ;
h = vl_plotframe(f1) ; set(h,'color','b','linewidth',2) ;
h = vl_plotframe(f0) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
vl_demo_print('sift_cmd_1') ;

figure(2) ; clf ;
imagesc(I) ; colormap(gray(256)) ; hold on ;
s      = m0(1,perm(1:20)) ;
s0     = m0(2,perm(1:20)) ;
[dr,a] = intersect(m1(1,:),s) ;
s1     = m1(2,a) ;

h = vl_plotsiftdescriptor(d(:,s),  f(:,s)  ) ; set(h,'color','g','linewidth',4) ;
h = vl_plotsiftdescriptor(d1(:,s1),f1(:,s1)) ; set(h,'color','b','linewidth',2) ;
h = vl_plotsiftdescriptor(d0(:,s0),f0(:,s0)) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
vl_demo_print('sift_cmd_2') ;

% --------------------------------------------------------------------
%                                                      Test descriptor
% --------------------------------------------------------------------

tmp=f0' ; tmp(:,1:2) = tmp(:,1:2) - 1 ;
save(p_tmp,'tmp','-ASCII') ;

[err,msg] = vl_demo_cmd('sift',['"' p_img ...
                    '" --output "ascii://' p_txt ...
                    '" --read-frames "ascii://' p_tmp ...
                    '" -v']) ;
disp(msg) ;

data      = load(p_txt,'-ASCII') ;
f1        = data(:,1:4)' ; f1(1:2,:) = f1(1:2,:) + 1 ;
d1        = uint8(data(:,5:end)') ;
[f,d]     = vl_sift(I,'FirstOctave',-1,'frames',f0,'verbose') ;

[m0, sc0]  = vl_ubcmatch(d,d0) ;
[m1, sc1]  = vl_ubcmatch(d,d1) ;
[dr,perm]  = sort(sc0,'ascend') ;

figure(3) ; clf ;
imagesc(I) ; colormap(gray(256)) ; hold on ;
h = vl_plotframe(f) ;  set(h,'color','g','linewidth',4) ;
h = vl_plotframe(f1) ; set(h,'color','b','linewidth',2) ;
h = vl_plotframe(f0) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
vl_demo_print('sift_cmd_3') ;

figure(4) ; clf ;
imagesc(I) ; colormap(gray(256)) ; hold on ;
s      = m0(1,perm(1:20)) ;
s0     = m0(2,perm(1:20)) ;
[dr,a] = intersect(m1(1,:),s) ;
s1     = m1(2,a) ;

h = vl_plotsiftdescriptor(d(:,s),  f(:,s)  ) ; set(h,'color','g','linewidth',4) ;
h = vl_plotsiftdescriptor(d1(:,s1),f1(:,s1)) ; set(h,'color','b','linewidth',2) ;
h = vl_plotsiftdescriptor(d0(:,s0),f0(:,s0)) ; set(h,'color','r','linewidth',1) ;

legend('MATLAB','command line','original SIFT') ;

axis equal ; axis off ; axis tight ;
vl_demo_print('sift_cmd_4') ;
