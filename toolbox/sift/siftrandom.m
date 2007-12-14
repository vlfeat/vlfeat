function [f d] = siftrandom(I, nfeatures, minsigma, orient)
% SIFTRANDOM  SIFT on random patches ?

if nargin < 4
    % By default, do not compute orientation, assume to be 0
    orient = 0;
end

I = im2double(I);
if size(I,3) > 1
    I = rgb2gray(I) ;
end
f = zeros(4,nfeatures);
H = size(I,1);
W = size(I,2);

rmin = 6*minsigma ;
rmax = min(H,W)/2 ;

% we select the size according to this distribution
r_range=rmin:rmax ;
p=H*W - 2*(H+W)*r_range +4*r_range.^2 ;
p=p./sum(p) ;
P=cumsum(p) ;
P=[0 P(1:end-1)] ;

for k=1:nfeatures
    rn=rand; 
    sel=find(P<rn) ;
    r=r_range(max(sel));

    w = W - 2*r ;
    h = H - 2*r  ;
    C = [w * rand + r ; h * rand + r] ;
    sigma = r / 6 ;

    f(:,k) = [C;sigma;0] ;
end

% SIFT features
if orient
    [f,d] =  sift(single(I), 'frames', f, 'orientations') ;
else
    [f,d] =  sift(single(I), 'frames', f) ;
end
