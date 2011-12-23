function [tpr,tnr,info] = vl_roc(labels, scores, varargin)
% VL_ROC Compute ROC curve
%  [TP,TN] = VL_ROC(LABELS, SCORES) computes the receiver operating
%  characteristic (ROC curve). LABELS are the ground thruth labels (+1
%  or -1) and SCORE is the scores assigned to them by a classifier
%  (higher scores correspond to positive labels).
%
%  [TP,TN] are the true positive and true negative rates for
%  incereasing values of the decision threshold.
%
%  Set the zero the lables of samples to ignore in the evaluation.
%
%  Set to -INF the score of samples which are never retrieved. In
%  this case the PR curve will have maximum recall < 1.
%
%  [TP,TN,INFO] = VL_ROC(...) returns the following additional
%  information:
%
%  INFO.EER:: Equal error rate.
%  INFO.AUC:: Area under the VL_ROC (AUC).
%  INFO.UR::  Uniform prior best op point rate.
%  INFO.UT::  Uniform prior best op point threhsold.
%  INFO.NR::  Natural prior best op point rate.
%  INFO.NT::  Natural prior best op point threshold.
%
%  VL_ROC(...) with no output arguments plots the VL_ROC diagram in
%  the current axis.
%
%  About the ROC curve::
%    Consider a classifier that predicts as positive all lables Y
%    whose SCORE is not smaller than a threshold S. The ROC curve
%    represents the performance of such classifier as the threshold S
%    is changed. Define
%
%      P = num. of positive samples,
%      N = num. of negative samples,
%
%    and for each threshold S
%
%      TP(S) = num. of samples that are correctly classified as positive,
%      TN(S) = num. of samples that are correctly classified as negative,
%      FP(S) = num. of samples that are incorrectly classified as positive,
%      FN(S) = num. of samples that are incorrectly classified as negative.
%
%    Consider also the rates:
%
%      TPR = TP(S) / P,      FNR = FN(S) / P,
%      TNR = TN(S) / N,      FPR = FP(S) / N,
%
%    and notice that by definition
%
%      P = TP(S) + FN(S) ,    N = TN(S) + FP(S),
%      1 = TPR(S) + FNR(S),   1 = TNR(S) + FPR(S).
%
%    The ROC curve is the parametric curve (TPR(S), TNR(S)) obtained
%    as the classifier threshold S is varied from -INF to +INF. The
%    TPR is also known as recall (see VL_PR()).
%
%    The ROC curve is contained in the square with vertices (0,0) The
%    (average) ROC curve of a random classifier is a line which
%    connects (1,0) and (0,1).
%
%    The ROC curve is independent of the prior probability of the
%    labels (i.e. of P/(P+N) and N/(P+N)).
%
%    An OPERATING POINT is a point on the ROC curve corresponding to
%    a certain threshold S. Each operating point corresponds to
%    minimizing the empirical 01 error of the classifier for given
%    prior probabilty of the labels. VL_ROC() computes the following
%    operating points:
%
%     Natural operating point:: Assumes P[Y=+1] = P / (P + N).
%     Uniform operating point:: Assumes P[Y=+1] = 1/2.
%
%   VL_ROC() acccepts the following options:
%
%   Plot:: []
%     Setting this option turns on plotting. Set to 'TrueNegative' or
%     'TN' to plot TP(S) (recall) vs. TN(S). Set to 'FalseNegative' or
%     'FN' to plot TP(S) (recall) vs. FP(S) = 1 - TN(S).
%
%  See also: VL_PR(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.plot = [] ;
opts = vl_argparse(opts, varargin) ;

% make row vectors
labels = labels(:)' ;
scores = scores(:)' ;

% sort by descending scores
[scores, perm] = sort(scores, 'descend') ;
labels = labels(perm) ;

% assume that data with -INF score is never retrieved
stop = max(find(scores > -inf)) ;

% Compute number of true positives, false positives, and overall
% peositives. Note that labels==0 don't increase any of the counts.
tp = [0 cumsum(labels(1:stop) > 0)] ;
fp = [0 cumsum(labels(1:stop) < 0)] ;
p = sum(labels > 0) ;
n = sum(labels < 0) ;

% compute the rates
tpr = tp / (p + eps) ;
fpr = fp / (n + eps) ;

fnr = 1 - tpr ;
tnr = 1 - fpr ;

% --------------------------------------------------------------------
%                                                      Additional info
% --------------------------------------------------------------------

if nargout > 0 | nargout == 0

  % equal error rate
  i1 = max(find(tpr >= tnr)) ;
  i2 = max(find(tnr >= tpr)) ;
  eer = 1 - max(tnr(i1), tpr(i2)) ;

  % uniform prior and natural prior operating points
  [drop, upoint] = max(tpr * .5 + tnr * .5) ;
  [drop, npoint] = max(tpr * p  + tnr * n) ;

  % uniform prior and natural prior operationg points rates and thresholds
  ur = tpr(upoint) * .5      + tnr(upoint) * .5 ;
  nr = tpr(npoint) * p/(p+n) + tnr(npoint) * n/(p+n) ;

  scores_ = [-inf, scores] ;
  ut = scores_(upoint) ;
  nt = scores_(npoint) ;

  % area
  area = sum((tnr(1:end-1)+tnr(2:end)) .* diff(tpr))/2 ;

  info.eer  = eer ;
  info.auc  = area ;
  info.ut   = ut ;
  info.ur   = ur ;
  info.nt   = nt ;
  info.nr   = nr ;
end

% --------------------------------------------------------------------
%                                                                 Plot
% --------------------------------------------------------------------

if ~isempty(opts.plot) || (nargout == 0)
  if isempty(opts.plot), opts.plot = 'tn' ; end
  cla ; hold on ;
  switch lower(opts.plot)
    case {'truenegatives', 'tn'}
      plot(tnr, tpr, 'b', 'linewidth', 2) ;
      spline((1-eer) * [0 1 1], (1-eer) * [1 1 0], 'r--') ;
      spline(tnr(upoint) * [0 1 1], tpr(upoint) * [1 1 0], 'g--') ;
      spline(tnr(npoint) * [0 1 1], tpr(npoint) * [1 1 0], 'k--') ;
      spline([0 1], [1 0], 'b:', 'linewidth', 2) ;
      spline([0 1], [0 1], 'y--', 'linewidth', 1) ;
      xlabel('true negative rate') ;
      ylabel('true positve rate (recall)') ;

    case {'falsepositives', 'fp'}
      plot(fpr, tpr, 'b', 'linewidth', 2) ;
      spline(eer * [0 1 1], (1-eer) * [1 1 0], 'r--') ;
      spline((1-tnr(upoint)) * [0 1 1], tpr(upoint) * [1 1 0], 'g--') ;
      spline((1-tnr(npoint)) * [0 1 1], tpr(npoint) * [1 1 0], 'k--') ;
      spline([1 0], [1 0], 'b:', 'linewidth', 2) ;
      spline([1 0], [0 1], 'y--', 'linewidth', 1) ;
      xlabel('false positive rate') ;
      ylabel('true positve rate (recall)') ;
    otherwise
      error('Invalid argument %s for option PLOT.', opts.plot);
  end

  grid on ;
  xlim([0 1]) ;
  ylim([0 1]) ;
  axis square ;
  title(sprintf('ROC (AUC = %.3g)', area), 'interpreter', 'none') ;
  legend('ROC', ...
         sprintf('eer %.3g %%', 100 * eer), ...
         sprintf('op. unif. %.3g %%', 100 * ur), ...
         sprintf('op. nat. %.3g %%', 100 * nr), ...
         'ROC rand.', 'Location', 'SouthWest') ;
end

function h = spline(x,y,spec,varargin)
prop = vl_linespec2prop(spec) ;
h = line(x,y,prop{:},varargin{:}) ;
