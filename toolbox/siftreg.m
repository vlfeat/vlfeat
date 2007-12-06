function [f d] = siftreg(I, spacing, patchwidth, varargin);
% spacing and patchwidth are both in pixels

orient = 0;
scales = 1;
normthresh = 0;
colorspace = 'gray';

for k=1:2:length(varargin)
    opt = lower(varargin{k}) ;
    arg = varargin{k+1} ;

    switch opt
        case 'orient'
            orient = arg;

        case 'scales'
            scales = arg;

        case 'normthresh'
            normthresh = arg;

        case 'colorspace'
            colorspace = arg;    

        otherwise
            error(['Uknown parameter ''', varargin{a}, '''.']) ;
    end      
end

I = im2double(I);
if size(I,3) > 1 && strcmp(colorspace, 'gray')
    I = rgb2gray(I) ;
elseif size(I,3) == 1 && ~strcmp(colorspace, 'gray')
    if strcmp(colorspace, 'normrgb')
        error('normrgb will not work with grayscale images');
    end
    I = cat(3,I,I,I);
end

if strcmp(colorspace, 'normrgb')
    RGB = (sum(I,3) + eps);
    I = I ./ cat(3,RGB,RGB,RGB);
    I = I(:,:,1:2); % B channel is redundant now
end

M = size(I,1);
N = size(I,2);

f = [];
for s = 1:scales
    sigma = patchwidth/2/6; %patch radius will be 6*sigma
    
    xpts = patchwidth/2:spacing:N-patchwidth/2;
    ypts = patchwidth/2:spacing:M-patchwidth/2;
    [X Y] = meshgrid(xpts, ypts);
    npts = length(X(:));
    f = [f; X(:) Y(:) sigma*ones(npts,1) zeros(npts,1)];

    patchwidth = patchwidth * 2;
    spacing = spacing * 2;
end

f = f';

allf = [];
alld = [];
for c = 1:size(I,3)
    % SIFT features
    if orient
        [f,d] =  sift(single(I(:,:,c)), 'frames', f, 'orientations', 'normthresh', normthresh) ;
    else
        [f,d] =  sift(single(I(:,:,c)), 'frames', f, 'normthresh', normthresh) ;
    end
    allf = f;
    alld = [alld; d];
end
f = allf;
d = alld;
