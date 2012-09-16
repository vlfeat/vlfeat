function im = vl_impattern(varargin)
% VL_IMPATTERN Generate an image from a stock pattern
%   IM=VLPATTERN(NAME) returns an instance of the specified
%   pattern. These stock patterns are useful for testing algoirthms.
%
%   All generated patterns are returned as an image of class
%   DOUBLE. Both gray-scale and colour images have range in [0,1].
%
%   VL_IMPATTERN() without arguments shows a gallery of the stock
%   patterns. The following patterns are supported:
%
%   Wedge::
%     The image of a wedge.
%
%   Cone::
%     The image of a cone.
%
%   SmoothChecker::
%     A checkerboard with Gaussian filtering on top. Use the
%     option-value pair 'sigma', SIGMA to specify the standard
%     deviation of the smoothing and the pair 'step', STEP to specfity
%     the checker size in pixels.
%
%   ThreeDotsSquare::
%     A pattern with three small dots and two squares.
%
%   UniformNoise::
%     Random i.i.d. noise.
%
%   Blobs:
%     Gaussian blobs of various sizes and skews.
%
%   A stock image::
%     Any of 'box', 'roofs1', 'roofs2', 'river1', 'river2', 'spotted'.
%
%   All pattern accept a SIZE parameter [WIDTH,HEIGHT]. For all but
%   the stock images, the default size is [128,128].

% Author: Andrea Vedaldi

% AUTORIGHTS

if nargin > 0
  pattern=varargin{1} ;
  varargin=varargin(2:end) ;
else
  pattern = 'gallery' ;
end

patterns = {'wedge','cone','smoothChecker','threeDotsSquare', ...
            'blobs', ...
            'box', 'roofs1', 'roofs2', 'river1', 'river2'} ;

% spooling
switch lower(pattern)
  case 'wedge', im = wedge(varargin) ;
  case 'cone', im = cone(varargin) ;
  case 'smoothchecker', im = smoothChecker(varargin) ;
  case 'threedotssquare', im = threeDotSquare(varargin) ;
  case 'uniformnoise', im = uniformNoise(varargin) ;
  case 'blobs', im = blobs(varargin) ;
  case {'box','roofs1','roofs2','river1','river2','spots'}
    im = stockImage(pattern, varargin) ;
  case 'gallery'
    clf ;
    num = numel(patterns) ;
    for p = 1:num
      vl_tightsubplot(num,p,'box','outer') ;
      imagesc(vl_impattern(patterns{p}),[0 1]) ;
      axis image off ;
      title(patterns{p}) ;
    end
    colormap gray ;
    return ;
  otherwise
    error('Unknown patter ''%s''.', pattern) ;
end

if nargout == 0
  clf ; imagesc(im) ; hold on ;
  colormap gray ; axis image off ;
  title(pattern) ;
  clear im ;
end

function [u,v,opts,args] = commonOpts(args)
opts.size = [128 128] ;
[opts,args] = vl_argparse(opts, args) ;
ur = linspace(-1,1,opts.size(2)) ;
vr = linspace(-1,1,opts.size(1)) ;
[u,v] = meshgrid(ur,vr);

function im = wedge(args)
[u,v,opts,args] = commonOpts(args) ;
im = abs(u) + abs(v) > (1/4) ;
im(v < 0) = 0 ;

function im = cone(args)
[u,v,opts,args] = commonOpts(args) ;
im = sqrt(u.^2+v.^2) ;
im = im / max(im(:)) ;

function im = smoothChecker(args)
opts.size = [128 128] ;
opts.step = 16 ;
opts.sigma = 2 ;
opts = vl_argparse(opts, args) ;
[u,v] = meshgrid(0:opts.size(1)-1, 0:opts.size(2)-1) ;
im = xor((mod(u,opts.step*2) < opts.step),...
         (mod(v,opts.step*2) < opts.step)) ;
im = double(im) ;
im = vl_imsmooth(im, opts.sigma) ;

function im = threeDotSquare(args)
[u,v,opts,args] = commonOpts(args) ;
im = ones(size(u)) ;
im(-2/3<u & u<2/3 & -2/3<v & v<2/3) = .75 ;
im(-1/3<u & u<1/3 & -1/3<v & v<1/3) = .50 ;
[drop,i] = min(abs(v(:,1))) ;
[drop,j1] = min(abs(u(1,:)-1/6)) ;
[drop,j2] = min(abs(u(1,:))) ;
[drop,j3] = min(abs(u(1,:)+1/6)) ;
im(i,j1) = 0 ;
im(i,j2) = 0 ;
im(i,j3) = 0 ;

function im = blobs(args)
[u,v,opts,args] = commonOpts(args) ;
im = zeros(size(u)) ;
num = 5 ;
square = 2 / num ;
sigma = square / 2 / 3 ;
scales = logspace(log10(0.5), log10(1), num) ;
skews = linspace(1,2,num) ;
for i=1:num
  for j=1:num
    cy = (i-1) * square + square/2 - 1;
    cx = (j-1) * square + square/2 - 1;
    A = sigma * diag([scales(i) scales(i)/skews(j)])  * [1 -1 ; 1 1] / sqrt(2)  ;
    C = inv(A'*A) ;
    x = u - cx ;
    y = v - cy ;
    im = im + exp(-0.5 *(x.*x*C(1,1) + y.*y*C(2,2) + 2*x.*y*C(1,2))) ;
  end
end
im = im / max(im(:)) ;

function im = uniformNoise(args)
opts.size = [128 128] ;
opts.seed = 1 ;
opts = vl_argparse(opts, args) ;
state = vl_twister('state') ;
vl_twister('state',opts.seed) ;
im = vl_twister(opts.size([2 1])) ;
vl_twister('state',state) ;

function im = stockImage(pattern,args)
opts.size = [] ;
opts = vl_argparse(opts, args) ;
switch pattern
  case 'river1',  path='river1.jpg' ;
  case 'river2',  path='river2.jpg' ;
  case 'roofs1',  path='roofs1.jpg' ;
  case 'roofs2',  path='roofs2.jpg' ;
  case 'box',     path='box.pgm' ;
  case 'spots',   path='spots.jpg' ;
end
im = imread(fullfile(vl_root,'data',path)) ;
im = im2double(im) ;
if ~isempty(opts.size)
  im = imresize(im, opts.size) ;
  im = max(im,0) ;
  im = min(im,1) ;
end
