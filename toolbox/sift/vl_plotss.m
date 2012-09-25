function plotss(ss, varargin)
% PLOTSS
%
%   Uniform:: false
%     If TRUE then use a fixed gray scale for all the levels.

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
