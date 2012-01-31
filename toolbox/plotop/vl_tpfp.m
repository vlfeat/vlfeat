function [tp, fp, p, n, perm, varargin] = vl_tpfp(labels, scores, varargin)
% VL_TPFP  Compute true positives and false positives
%  This is an helper function used by VL_PR(), VL_ROC(), VL_DET().
%
%  See also: VL_PR(), VL_ROC(), VL_DET(), VL_HELP().

% Author: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% extraNeg and extraPos depend on numNegatives and numPositives:
%
%    [           labels            |    -1       +1    ]
%    [ +inf | finite scores | -inf | extraNeg  extraPos]

opts.includeInf = false ;
opts.numNegatives = [] ;
opts.numPositives = [] ;
[opts, varargin] = vl_argparse(opts, varargin) ;

% make row vectors
labels = labels(:)' ;
scores = scores(:)' ;

% count labels
p = sum(labels > 0) ;
n = sum(labels < 0) ;

if ~isempty(opts.numPositives)
  if opts.numPositives < p
    warning('NUMPOSITIVES is smaller than the number of positives in LABELS.') ;
  end
  p = opts.numPositives ;
end

if ~isempty(opts.numNegatives)
  if opts.numNegatives < n
    warning('NUMNEGATIVES is smaller than the number of negatives in LABELS.') ;
  end
  n = opts.numNegatives ;
end

% sort by descending scores
[scores, perm] = sort(scores, 'descend') ;

% assume that data with -INF score is never retrieved
if opts.includeInf
  stop = length(scores) ;
else
  stop = max(find(scores > -inf)) ;
end
perm = perm(1:stop) ;
labels = labels(perm) ;

% accumulate true positives and false positives by scores
% in descending order
tp = [0 cumsum(labels > 0)] ;
fp = [0 cumsum(labels < 0)] ;
