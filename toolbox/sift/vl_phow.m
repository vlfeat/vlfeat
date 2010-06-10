function [frames, descrs] = vl_phow(im, varargin)
% VL_PHOW  Extract PHOW features
%   [FRAMES, DESCRS] = VL_PHOW(IM) extracts PHOW features from the
%   image IM. This function is a wrapper around VL_DSIFT() and
%   VL_IMSMOOTH().
%
%   The PHOW descriptors where introduced in [1]. By default,
%   VL_PHOW() computes the gray-scale variant of the descriptor.  The
%   COLOR option can be used to compute the color variant instead.
%
%   Verbose:: false
%     Set to true to turn on verbose output.
%
%   Sizes:: [4 6 8 10]
%     Scales at which the dense SIFT features are extracted. Each
%     value is used as bin size for the VL_DSIFT() function.
%
%   Fast:: true
%     Set to false to turn off the fast SIFT features computation by
%     VL_DSIFT().
%
%   Step:: 2
%     Step (in pixels) of the grid at which the dense SIFT features
%     are extracted.
%
%   Color:: false
%     Set to true to compute PHOW-color rather than PHOW-gray.
%
%   ContrastThreshold:: 0.005
%     Contrast threshold below which SIFT features are mapped to
%     zero. The input image is scaled to have intensity range in [0,1]
%     (rather than [0,255]) and this value is compared to the
%     descriptor norm as returned by VL_DSIFT().
%
%   WindowSize:: 1.5
%     Size of the Gaussian window in units of spatial bins.
%
%   Magnif:: 6
%     The image is smoothed by a Gaussian kernel of standard deviation
%     SIZE / MAGNIF.
%
%   See also: VL_HELP(), VL_DSIFT().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

% -------------------------------------------------------------------
%                                                 Parse the arguments
% -------------------------------------------------------------------

  opts.verbose = false ;
  opts.fast = true ;
  opts.sizes = [4 6 8 10] ;
  opts.step = 2 ;
  opts.color = false ;
  opts.magnif = 6 ;
  opts.windowsize = 1.5 ;
  opts.contrastthreshold = 0.005 ;
  opts = vl_argparse(opts,varargin) ;

  dsiftOpts = {'norm', 'windowsize', opts.windowsize} ;
  if opts.verbose, dsiftOpts{end+1} = 'verbose' ; end
  if opts.fast, dsiftOpts{end+1} = 'fast' ; end
  dsiftOpts(end+(1:2)) = {'step', opts.step} ;

% -------------------------------------------------------------------
%                                                Extract the features
% -------------------------------------------------------------------


  % standarize the image
  imageSize = [size(im,2) ; size(im,1)] ;
  if opts.color
    if size(im,3) == 1, im = cat(3, im, im, im) ; end
    im = rgb2hsv(im) ;
    numChannels = 3 ;
  else
    if size(im,3) == 3, im = rgb2gray(im) ; end
    numChannels = 1 ;
  end

  if opts.verbose
    fprintf('%s: image size: %d x %d\n', mfilename, imageSize(1), imageSize(2)) ;
    fprintf('%s: sizes: [%s]\n', mfilename, sprintf(' %d', opts.sizes)) ;
  end

  for si = 1:length(opts.sizes)

    % Recall from VL_DSIFT() that the first descriptor for scale SIZE has
    % center located at XC = XMIN + 3/2 SIZE (the Y coordinate is
    % similar). It is convenient to align the descriptors at different
    % scales so that they have the same geometric centers. For the
    % maximum size we pick XMIN = 1 and we get centers starting from
    % XC = 1 + 3/2 MAX(OPTS.SIZES). For any other scale we pick XMIN so
    % that XMIN + 3/2 SIZE = 1 + 3/2 MAX(OPTS.SIZES).
    %
    % In pracrice, the offset must be integer ('bounds'), so the
    % alignment works properly only if all OPTS.SZES are even or odd.

    off = floor(1 + 3/2 * (max(opts.sizes) - opts.sizes(si))) ;

    % scale space
    sigma = opts.sizes(si) / opts.magnif ;
    ims = vl_imsmooth(im, sigma) ;

    for k = 1:numChannels
      [frames{si}, descrs{k,si}] = vl_dsift(...
        ims(:,:,k), ...
        dsiftOpts{:},  ...
        'size', opts.sizes(si), ...
        'bounds', [off off +inf +inf]) ;
    end

    % remove low contrast descriptors
    % note that for color descriptors the V component is thresholded
    for k = 1:numChannels
      descrs{k,si}(:, frames{si}(3,:) < opts.contrastthreshold) = 0 ;
    end

    % save only x,y, and the scale
    frames{si} = [frames{si}(1:3, :) ; opts.sizes(si) * ones(1,size(frames{si},2))] ;
  end
  descrs = cell2mat(descrs) ;
  frames = cell2mat(frames) ;
end
