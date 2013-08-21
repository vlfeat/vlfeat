function features = getDenseSIFT(im, varargin)
% GETDENSESIFT   Extract dense SIFT features
%   FEATURES = GETDENSESIFT(IM) extract dense SIFT features from
%   image IM.

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.scales = logspace(log10(1), log10(.25), 5) ;
opts.contrastthreshold = 0 ;
opts.step = 3 ;
opts.rootSift = false ;
opts.normalizeSift = true ;
opts.binSize = 8 ;
opts.geometry = [4 4 8] ;
opts.sigma = 0 ;
opts = vl_argparse(opts, varargin) ;

dsiftOpts = {'norm', 'fast', 'floatdescriptors', ...
             'step', opts.step, ...
             'size', opts.binSize, ...
             'geometry', opts.geometry} ;

if size(im,3)>1, im = rgb2gray(im) ; end
im = im2single(im) ;
im = vl_imsmooth(im, opts.sigma) ;

for si = 1:numel(opts.scales)
  im_ = imresize(im, opts.scales(si)) ;

  [frames{si}, descrs{si}] = vl_dsift(im_, dsiftOpts{:}) ;

  % root SIFT
  if opts.rootSift
    descrs{si} = sqrt(descrs{si}) ;
  end
  if opts.normalizeSift
    descrs{si} = snorm(descrs{si}) ;
  end

  % zero low contrast descriptors
  info.contrast{si} = frames{si}(3,:) ;
  kill = info.contrast{si} < opts.contrastthreshold  ;
  descrs{si}(:,kill) = 0 ;

  % store frames
  frames{si}(1:2,:) = (frames{si}(1:2,:)-1) / opts.scales(si) + 1 ;
  frames{si}(3,:) = opts.binSize / opts.scales(si) / 3 ;
end

features.frame = cat(2, frames{:}) ;
features.descr = cat(2, descrs{:}) ;
features.contrast = cat(2, info.contrast{:}) ;

function x = snorm(x)
x = bsxfun(@times, x, 1./max(1e-5,sqrt(sum(x.^2,1)))) ;
