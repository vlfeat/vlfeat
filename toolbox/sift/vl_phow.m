function [frames, descrs] = vl_phow(im, varargin)
% VL_PHOW  Extract PHOW features
%   [FRAMES, DESCRS] = VL_PHOW(IM) extracts PHOW features [1] from the
%   image IM. PHOW is simply dense SIFT applied at several resolutions. This function is a commodity interface to VL_DSIFT() and
%   VL_IMSMOOTH().
%
%   DESCRS has the same format of VL_SIFT() and VL_DSIFT(). FRAMES(1:2,:)
%   are the x,y coordinates of the center of each descriptor, FRAMES(3,:)
%   is the contrast of the descriptor, as returned by VL_DSIFT() (for
%   colour variant, contranst is computed on the intensity channel).
%   FRAMES(4,:) is the size of the bin of the descriptor.
%
%   By default,
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
%   Color:: 'gray'
%     Choose between 'gray' (PHOW-gray), 'rgb', 'hsv', and 'opponent'
%     (PHOW-color).
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
%     SIZE / MAGNIF. Note that, in the standard SIFT descriptor, the
%     magnification value is 3; here the default one is 6 as it seems
%     to perform better in applications.
%
%   FloatDescriptors:: false
%     If set to TRUE, the descriptors are returned in floating point
%     format.
%
%   See also: VL_DSIFT(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% -------------------------------------------------------------------
%                                                 Parse the arguments
% -------------------------------------------------------------------

  opts.verbose = false ;
  opts.fast = true ;
  opts.sizes = [4 6 8 10] ;
  opts.step = 2 ;
  opts.color = 'gray' ;
  opts.floatdescriptors = false ;
  opts.magnif = 6 ;
  opts.windowsize = 1.5 ;
  opts.contrastthreshold = 0.005 ;
  opts = vl_argparse(opts,varargin) ;

  dsiftOpts = {'norm', 'windowsize', opts.windowsize} ;
  if opts.verbose, dsiftOpts{end+1} = 'verbose' ; end
  if opts.fast, dsiftOpts{end+1} = 'fast' ; end
  if opts.floatdescriptors, dsiftOpts{end+1} = 'floatdescriptors' ; end
  dsiftOpts(end+(1:2)) = {'step', opts.step} ;

% -------------------------------------------------------------------
%                                                Extract the features
% -------------------------------------------------------------------


  % standarize the image
  imageSize = [size(im,2) ; size(im,1)] ;
  if strcmp(lower(opts.color), 'gray')
    numChannels = 1 ;
    if size(im,3) > 1, im = rgb2gray(im) ; end
  else
    numChannels = 3 ;
    if size(im,3) == 1, im = cat(3, im, im, im) ; end
    switch lower(opts.color)
      case 'rgb'
      case 'opponent'
        % Note that the mean differs from the standard definition of opponent
        % space and is the regular intesity (for compatibility with
        % the contrast thresholding).
        %
        % Note also that the mean is added pack to the other two
        % components with a small multipliers for monochromatic
        % regions.
        mu = 0.3*im(:,:,1) + 0.59*im(:,:,2) + 0.11*im(:,:,3) ;
        alpha = 0.01 ;
        im = cat(3, mu, ...
                 (im(:,:,1) - im(:,:,2))/sqrt(2) + alpha*mu, ...
                 (im(:,:,1) + im(:,:,2) - 2*im(:,:,3))/sqrt(6) + alpha*mu) ;
      case 'hsv'
        im = rgb2hsv(im) ;
      otherwise
        opts.color = 'hsv' ;
        warning('Color space not recongized, defaulting to HSV color space.') ;
    end
  end

  if opts.verbose
    fprintf('%s: color space: %s\n', mfilename, opts.color) ;
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

    % smooth the image to the appropriate scale based on the size
    % of the SIFT bins
    sigma = opts.sizes(si) / opts.magnif ;
    ims = vl_imsmooth(im, sigma) ;

    % extract dense SIFT features from all channels
    for k = 1:numChannels
      [f{k}, d{k}] = vl_dsift(...
        ims(:,:,k), ...
        dsiftOpts{:},  ...
        'size', opts.sizes(si), ...
        'bounds', [off off +inf +inf]) ;
    end

    % remove low contrast descriptors
    % note that for color descriptors the V component is
    % thresholded
    switch lower(opts.color)
      case {'gray', 'opponent'}
        contrast = f{1}(3,:) ;
      case 'rgb'
        contrast = mean([f{1}(3,:) ; f{2}(3,:) ; f{3}(3,:)],1) ;
      otherwise % hsv
        contrast = f{3}(3,:) ;
    end
    for k = 1:numChannels
      d{k}(:, contrast < opts.contrastthreshold) = 0 ;
    end

    % save only x,y, and the scale
    frames{si} = [f{1}(1:3, :) ; opts.sizes(si) * ones(1,size(f{1},2))] ;
    descrs{si} = cat(1, d{:}) ;
  end
  descrs = cell2mat(descrs) ;
  frames = cell2mat(frames) ;
end
