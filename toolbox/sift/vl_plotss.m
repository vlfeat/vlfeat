function vl_plotss(ss, varargin)
% VL_PLOTSS Plot scale space
%   VL_PLOTSS(SS) plots the scale space SS. SS is a structure
%   with the following members:
%
%   ss.firstOctave::
%     The index of the first octave in the scale space.
%
%   ss.lastOctave::
%     The index of the last octave in the scale space.
%
%   ss.octaveResolution::
%     The octave resolution, i.e. the nubmer of subdivisions
%     per octave.
%
%   ss.octaveFirstLevel::
%     The index of the first level of subdivisions for each octave.
%
%   ss.octaveLastLevel::
%     The iundex of last leve of subdivisions for each cotave.
%
%   ss.data::
%     A cell array of 3D arrays representing the scale space data.
%     The cell array has a length equal to the nubmer of octaves
%     contained in the scale space. Each entry is a 3D array, the
%     first two dimensions of which correspond to image rows and
%     columns respectively, and the third to scale levels.
%
%   ss.sigma0::
%     Base smoothing.
%
%   A scale space is a representation of a 2D signal (image) at
%   multiple scales. In the simplest case, a scale SIGMA is defined as
%   the input image I(x,y) convolved by a Gaussian kernel of isotropic
%   standard deviation SIGMA:
%
%     I(x,y;sigma) = (g_sigma * I)(x,y)
%
%   where scales are sampled as follows:
%
%     sigma(o,s) = sigma0 2^{o + s / ss.octaveResolution),
%     ss.firstOctave <= o <= ss.lastOctave,
%     ss.octaveFirstLeve <= s <= ss.octaveLastLevel.
%
%   Moving from one octave to the next, the size of the kernel
%   doubles. Hence the effective bandwith of the signal halves, and
%   resolution can be reduced by half. Typically, for example, sigma0
%   = 1.6, so at octave 0 the image can be effectively sampled with a
%   step of 1, and the resolution of octave 0 is the same as the one
%   at which the input image is presented. Then at octave o the
%   sampling step is 2^o.
%
%   ss.octaveResolution is the number of scale subdivisions per
%   octave. ss.firstOctave and ss.lastOctave give the additional
%   flexibility of specifying a range for the level index s to exceed
%   the standard setting [0, ss.octaveResolution-1]. In this manner
%   the same scales can be represented twice, at two sampling
%   rates. This is often convenient in feature computation (e.g. to
%   find local maxima in scale of a function).
%
%   VL_PLOTSS(SS, 'Option', value) supports the following options:
%
%   Uniform:: false
%     If TRUE then use a fixed gray scale for all the levels.

% Author: Andrea Vedaldi

% Copyright (C) 2007-12 Karel Lenc, Andrea Vedaldi, and Michal Perdoch
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.uniform = false ;
opts.clim = [] ;
opts = vl_argparse(opts, varargin) ;

numOctaves = numel(ss.data) ;

if opts.uniform & isempty(opts.clim)
  minv = +inf ;
  maxv = -inf ;
  for i = 1:numOctaves
    minv = min(minv, min(ss.data{i}(:))) ;
    maxv = max(maxv, max(ss.data{i}(:))) ;
  end
  opts.clim = [minv maxv] ;
end

clf ;
for i = 1:numOctaves
  vl_tightsubplot(numOctaves, i) ;
  vl_imarraysc(ss.data{i}, 'clim', opts.clim) ;
  axis image off ;
  title(sprintf('octave %d', ss.firstOctave - 1 + i)) ;
end
