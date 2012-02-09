function [recall, precision, info, scores] = vl_pr(labels, scores, varargin)
% VL_PR Compute precision-recall curve
%  [RECALL, PRECISION] = VL_PR(LABELS, SCORES) computes the
%  precision-recall (PR) curve. LABELS are the ground thruth labels
%  (+1 or -1) and SCORES are the scores associated to them by a
%  classifier (lager scores correspond to positive guesses).
%
%  RECALL and PRECISION are the recall and the precision for
%  increasing values of the decision threshold.
%
%  Set the zero the lables of data points to ignore in the evaluation.
%
%  Set to -INF the score of data points which are never retrieved. In
%  this case the PR curve will have maximum recall < 1.
%
%  VL_PR() accepts the following options:
%
%  InludeInf:: [false]
%    If set to true, data with -INF score is included in the
%    evaluation and the maximum recall is 1 even if -INF scores are
%    present.
%
%  Stable:: [false]
%    If set to true, RECALL and PRECISION are in the samre order of
%    LABELS and SCORES rather than being sorted by increasing
%    RECALL. This option implies INCLUDEINF.
%
%  About the PR curve::
%    This section uses the same symbols used in the documentation of
%    the VL_ROC() function. In addition to those quantities, define:
%
%      PRECISION(S) = TP(S) / (TP(S) + FP(S))
%      RECALL(S) = TP(S) / P = TPR(S)
%
%    The precision is the fraction of positivie predictions which are
%    correct, and the recall is the fraction of positive labels that
%    have been correctly classified (recalled). Notice that the
%    recall is also equal to the true positive rate for the ROC curve
%    (see VL_ROC()).
%
%  Remark::
%    Precision (P) is undefined for those values of the
%    classifier threshold for which no example is classified as
%    positive. Conventionally, a precision of P=1 is assigned to such
%    cases.
%
%  See also: VL_ROC(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

opts.includeInf = false ;
opts.stable = false ;
opts = vl_argparse(opts,varargin) ;

% make row vectors
labels = labels(:)' ;
scores = scores(:)' ;

% sort by descending scores
[scores, perm] = sort(scores, 'descend') ;
labels = labels(perm) ;

% assume that data with -INF score is never retrieved
if opts.includeInf | opts.stable
  stop = length(scores) ;
else
  stop = max(find(scores > -inf)) ;
end

% Compute number of true positives, false positives, and overall
% positives. Note that lables==0 do not increase neither TP nor FP nor
% affect P.
tp = [0 cumsum(labels(1:stop) == +1)] ;
fp = [0 cumsum(labels(1:stop) == -1)] ;
p = sum(labels == +1) ;

% compute precision and recall
recall = tp / (p + eps) ;
precision = (tp + eps) ./ (tp + fp + eps) ;

% compute AUC
if length(recall) > 1
  auc = sum((precision(1:end-1) + precision(2:end)) .* diff(recall)) / 2 ;
else
  auc = 0 ;
end

% compute AUC according to TRECVID / PASCAL VOC <= 2009
ap = 0;
for t=0 : 0.1 : 1
  p_ = max(precision(recall>=t));
  if isempty(p_)
    p_=0;
  end
  ap = ap + p_ / 11 ;
end

info.auc = auc ;
info.auc_pa08 = ap ;

% make a figure if there are no output arguments
if nargout == 0
  cla ; hold on ;
  plot(recall,precision,'linewidth',2) ;
  line([0 1], [1 1] * p / length(labels), 'color', 'r', 'linestyle', '--') ;
  axis square ; grid on ;
  xlim([0 1]) ; xlabel('recall') ;
  ylim([0 1]) ; ylabel('precision') ;
  title(sprintf('Precision-recall (AP = %.2f %%)', info.auc * 100)) ;
  legend('PR', 'random classifier', 'Location', 'NorthWestOutside') ;
  clear recall precision info ;
end

if opts.stable
  recall(1) = [] ;
  precision(1) = [] ;
  recall(perm) = recall ;
  precision(perm) = precision ;
  scores(perm(1:stop)) = scores(1:stop) ;
else
  scores = [+inf scores(1:stop)] ;
end
