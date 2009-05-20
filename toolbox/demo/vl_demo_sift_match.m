% VL_DEMO_SIFT_MATCH  Demo: SIFT: basic matching

pfx = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                    Create image pair
% --------------------------------------------------------------------

Ia = imread(fullfile(vl_root,'data','a.jpg')) ;

th = pi/4 ;
sc = 4 ;
c = sc*cos(th) ;
s = sc*sin(th) ;
A = [c -s; s c] ;
T = [- size(Ia,2) ; - size(Ia,1)]  / 2 ;

tform = maketform('affine', [A, A * T - T ; 0 0  1]') ;
Ib = imtransform(Ia,tform,'size',size(Ia), ...
                 'xdata', [1 size(Ia,2)], ...
                 'ydata', [1 size(Ia,1)], ...
                 'fill', 255);

% --------------------------------------------------------------------
%                                           Extract features and match
% --------------------------------------------------------------------

[fa,da] = vl_sift(im2single(rgb2gray(Ia))) ;
[fb,db] = vl_sift(im2single(rgb2gray(Ib))) ;

[matches, scores] = vl_ubcmatch(da,db) ;

[drop, perm] = sort(scores, 'descend') ;
matches = matches(:, perm) ;
scores  = scores(perm) ;

figure(1) ; clf ;
imagesc(cat(2, Ia, Ib)) ;

xa = fa(1,matches(1,:)) ;
xb = fb(1,matches(2,:)) + size(Ia,2) ;
ya = fa(2,matches(1,:)) ;
yb = fb(2,matches(2,:)) ;

hold on ;
h = line([xa ; xb], [ya ; yb]) ;
set(h,'linewidth', 2, 'color', 'b') ;

vl_plotframe(fa(:,matches(1,:))) ;
fb(1,:) = fb(1,:) + size(Ia,2) ;
vl_plotframe(fb(:,matches(2,:))) ;
axis equal ;
axis off  ;

vl_demo_print('sift_match_1', 1) ;
