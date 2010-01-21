function results = vl_test_sift(varargin)
% VL_TEST_SIFT
vl_test_init ;

function s = setup()
s.I = im2single(imread(fullfile(vl_root,'data','box.pgm'))) ;
[s.ubc.f, s.ubc.d] = ...
    vl_ubcread(fullfile(vl_root,'data','box.sift')) ;

function test_ubc_descriptor(s)
err = [] ;
[f, d] = vl_sift(s.I,...
                 'firstoctave', -1, ...
                 'frames', s.ubc.f) ;
D2 = vl_alldist(f, s.ubc.f) ;
[drop, perm] = min(D2) ;
f = f(:,perm) ;
d = d(:,perm) ;
error = mean(sqrt(sum((single(s.ubc.d) - single(d)).^2))) ...
        / mean(sqrt(sum(single(s.ubc.d).^2))) ;
assert(error < 0.1, ...
       'sift descriptor did not produce desctiptors similar to UBC ones') ;

function test_ubc_detector(s)
[f, d] = vl_sift(s.I,...
                 'firstoctave', -1, ...
                 'peakthresh', .01, ...
                 'edgethresh', 10) ;

s.ubc.f(4,:) = mod(s.ubc.f(4,:), 2*pi) ;
f(4,:) = mod(f(4,:), 2*pi) ;

% scale the components so that 1 pixel erro in x,y,z is equal to a
% 10-th of angle.
S = diag([1 1 1 20/pi]);
D2 = vl_alldist(S * s.ubc.f, S * f) ;
[d2,perm] = sort(min(D2)) ;
error = sqrt(d2) ;
quant80 = round(.8 * size(f,2)) ;

% check for less than one pixel error at 80% quantile
assert(error(quant80) < 1, ...
       'sift detector did not produce enough keypoints similar to UBC ones') ;



