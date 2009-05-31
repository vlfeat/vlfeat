% VL_DEMO_SIFT_VS_UBC Compare VLFeat SIFT with Lowe's original

% --------------------------------------------------------------------
%                            Load a figure and original SIFT keypoints
% --------------------------------------------------------------------

im = imread(fullfile(vl_root, 'data', 'box.pgm')) ;
[f1,d1] = vl_ubcread(fullfile(vl_root, 'data', 'box.sift')) ;

% --------------------------------------------------------------------
%                                    Compare with VLFeat SIFT detector
% --------------------------------------------------------------------

f2 = vl_sift(im2single(im), ...
             'firstoctave', -1, ...
             'peakthresh', .01, ...
             'edgethresh', 10, ...
             'windowsize', 2, ...
             'verbose') ;

D = sqrt(vl_alldist2(f1(1:2,:), f2(1:2,:))) ;
[d12,m12] = min(D,[],2) ;
[d21,m21] = min(D,[],1) ;

matches = [1:size(f1,2), m21 ; m12',  1:size(f2,2)] ;
proxim  = [d12', d21] ;

h = histc(proxim, [0 .01 .05 +inf]) ;
h = h / sum(h) * 100 ;
h = h(1:end-1) ;

figure(1) ; clf ;
imagesc(im) ; colormap(gray(256)) ; hold on ;
vl_plotframe(f1, 'linewidth', 3, 'color', 'r') ;
vl_plotframe(f2, 'linewidth',  1, 'color', 'b') ;
axis off ;
vl_demo_print('sift_vs_ubc_1') ;

figure(2) ; clf ;
pie(h) ;
colormap hot(3) ;
legend({'0.01 pixels', '0.05 pixels', 'others'}, ...
       'location', 'northeastoutside') ;
set(findobj(2, '-property', 'fontsize'), 'fontsize', 11) ;
vl_demo_print('sift_vs_ubc_2') ;

% --------------------------------------------------------------------
%                                  Compare with VLFeat SIFT descriptor
% --------------------------------------------------------------------

[drop,d2]=vl_sift(im2single(im), 'frames', f1, 'verbose', 'firstoctave', -1) ;

D = sqrt(double(vl_alldist2(d1, d2, 'l2'))) ;
[d12,m12] = min(D,[],2) ;
[d21,m21] = min(D,[],1) ;

matches = [1:size(d1,2), m21 ; m12',  1:size(d2,2)] ;
proxim  = [d12', d21] ;

meanDist = mean(D(:)) ;

h = histc(proxim, meanDist * [0 .05 .10 .20 +inf]) ;
h = h / sum(h) * 100 ;
h = h(1:end-1) ;

figure(3) ; clf ;
pie(h) ;
colormap hot(4) ;
legend({'5% differnce', '10% difference', '20% difference', 'others'}, ...
       'location', 'northeastoutside') ;
set(findobj(3, '-property', 'fontsize'), 'fontsize', 11) ;
vl_demo_print('sift_vs_ubc_3') ;
