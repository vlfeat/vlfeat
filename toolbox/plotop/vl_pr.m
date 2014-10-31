function [recall, precision, info] = vl_pr(labels, scores, varargin)
%VL_PR   Precision-recall curve.
%   [RECALL, PRECISION] = VL_PR(LABELS, SCORES) computes the
%   precision-recall (PR) curve. LABELS are the ground truth labels,
%   greather than zero for a positive sample and smaller than zero for
%   a negative one. SCORES are the scores of the samples obtained from
%   a classifier, where lager scores should correspond to positive
%   samples.
%
%   Samples are ranked by decreasing scores, starting from rank 1.
%   PRECISION(K) and RECALL(K) are the precison and recall when
%   samples of rank smaller or equal to K-1 are predicted to be
%   positive and the remaining to be negative. So for example
%   PRECISION(3) is the percentage of positive samples among the two
%   samples with largest score. PRECISION(1) is the precision when no
%   samples are predicted to be positive and is conventionally set to
%   the value 1.
%
%   Set to zero the lables of samples that should be ignored in the
%   evaluation. Set to -INF the scores of samples which are not
%   retrieved. If there are samples with -INF score, then the PR curve
%   may have maximum recall smaller than 1, unless the INCLUDEINF
%   option is used (see below). The options NUMNEGATIVES and
%   NUMPOSITIVES can be used to add additional surrogate samples with
%   -INF score (see below).
%
%   [RECALL, PRECISION, INFO] = VL_PR(...) returns an additional
%   structure INFO with the following fields:
%
%   info.auc::
%     The area under the precision-recall curve. If the INTERPOLATE
%     option is set to FALSE, then trapezoidal interpolation is used
%     to integrate the PR curve. If the INTERPOLATE option is set to
%     TRUE, then the curve is piecewise constant and no other
%     approximation is introduced in the calculation of the area. In
%     the latter case, INFO.AUC is the same as INFO.AP.
%
%   info.ap::
%     Average precision as defined by TREC. This is the average of the
%     precision observed each time a new positive sample is
%     recalled. In this calculation, any sample with -INF score
%     (unless INCLUDEINF is used) and any additional positive induced
%     by NUMPOSITIVES has precision equal to zero. If the INTERPOLATE
%     option is set to true, the AP is computed from the interpolated
%     precision and the result is the same as INFO.AUC. Note that AP
%     as defined by TREC normally does not use interpolation [1].
%
%   info.ap_interp_11::
%     11-points interpolated average precision as defined by TREC.
%     This is the average of the maximum precision for recall levels
%     greather than 0.0, 0.1, 0.2, ..., 1.0. This measure was used in
%     the PASCAL VOC challenge up to the 2008 edition.
%
%   info.auc_pa08::
%     Deprecated. It is the same of INFO.AP_INTERP_11.
%
%   VL_PR(...) with no output arguments plots the PR curve in the
%   current axis.
%
%   VL_PR() accepts the following options:
%
%   Interpolate:: false
%     If set to true, use interpolated precision. The interpolated
%     precision is defined as the maximum precision for a given recall
%     level and onwards. Here it is implemented as the culumative
%     maximum from low to high scores of the precision.
%
%   NumPositives:: []
%   NumNegatives:: []
%     If set to a number, pretend that LABELS contains this may
%     positive/negative labels. NUMPOSITIVES/NUMNEGATIVES cannot be
%     smaller than the actual number of positive/negative entrires in
%     LABELS. The additional positive/negative labels are appended to
%     the end of the sequence, as if they had -INF scores (not
%     retrieved). This is useful to evaluate large retrieval systems
%     for which one stores ony a handful of top results for efficiency
%     reasons.
%
%   IncludeInf:: false
%     If set to true, data with -INF score SCORES is included in the
%     evaluation and the maximum recall is 1 even if -INF scores are
%     present. This option does not include any additional positive or
%     negative data introduced by specifying NUMPOSITIVES and
%     NUMNEGATIVES.
%
%   Stable:: false
%     If set to true, RECALL and PRECISION are returned in the same order
%     of LABELS and SCORES rather than being sorted by decreasing
%     score (increasing recall). Samples with -INF scores are assigned
%     RECALL and PRECISION equal to NaN.
%
%   NormalizePrior:: []
%     If set to a scalar, reweights positive and negative labels so
%     that the fraction of positive ones is equal to the specified
%     value. This computes the normalised PR curves of [2]
%
%   About the PR curve::
%     This section uses the same symbols used in the documentation of
%     the VL_ROC() function. In addition to those quantities, define:
%
%       PRECISION(S) = TP(S) / (TP(S) + FP(S))
%       RECALL(S) = TPR(S) = TP(S) / P
%
%     The precision is the fraction of positivie predictions which are
%     correct, and the recall is the fraction of positive labels that
%     have been correctly classified (recalled). Notice that the recall
%     is also equal to the true positive rate for the ROC curve (see
%     VL_ROC()).
%
%   REFERENCES:
%   [1] C. D. Manning, P. Raghavan, and H. Schutze. An Introduction to
%   Information Retrieval. Cambridge University Press, 2008.
%   [2] D. Hoiem, Y. Chodpathumwan, and Q. Dai. Diagnosing error in
%   object detectors. In Proc. ECCV, 2012.
%
%   See also VL_ROC(), VL_HELP().

% Author: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

% TP and FP are the vectors of true positie and false positve label
% counts for decreasing scores, P and N are the total number of
% positive and negative labels. Note that if certain options are used
% some labels may actually not be stored explicitly by LABELS, so P+N
% can be larger than the number of element of LABELS.

[tp, fp, p, n, perm, varargin] = vl_tpfp(labels, scores, varargin{:}) ;
opts.stable = false ;
opts.interpolate = false ;
opts.normalizePrior = [] ;
opts = vl_argparse(opts,varargin) ;

% compute precision and recall
small = 1e-10 ;
recall = tp / max(p, small) ;
if isempty(opts.normalizePrior)
  precision = max(tp, small) ./ max(tp + fp, small) ;
else
  a = opts.normalizePrior ;
  precision = max(tp * a/max(p,small), small) ./ ...
      max(tp * a/max(p,small) + fp * (1-a)/max(n,small), small) ;
end

% interpolate precision if needed
if opts.interpolate
  precision = fliplr(vl_cummax(fliplr(precision))) ;
end

% --------------------------------------------------------------------
%                                                      Additional info
% --------------------------------------------------------------------

if nargout > 2 || nargout == 0

  % area under the curve using trapezoid interpolation
  if ~opts.interpolate
    info.auc = 0.5 * sum((precision(1:end-1) + precision(2:end)) .* diff(recall)) ;
  end

  % average precision (for each recalled positive sample)
  sel = find(diff(recall)) + 1 ;
  info.ap = sum(precision(sel)) / p ;
  if opts.interpolate
    info.auc = info.ap ;
  end

  % TREC 11 points average interpolated precision
  info.ap_interp_11 = 0.0 ;
  for rc = linspace(0,1,11)
    pr = max([0, precision(recall >= rc)]) ;
    info.ap_interp_11 = info.ap_interp_11 + pr / 11 ;
  end

  % legacy definition
  info.auc_pa08 = info.ap_interp_11 ;
end

% --------------------------------------------------------------------
%                                                                 Plot
% --------------------------------------------------------------------

if nargout == 0
  cla ; hold on ;
  plot(recall,precision,'linewidth',2) ;
  if isempty(opts.normalizePrior)
    randomPrecision = p / (p + n) ;
  else
    randomPrecision = opts.normalizePrior ;
  end
  spline([0 1], [1 1] * randomPrecision, 'r--', 'linewidth', 2) ;
  axis square ; grid on ;
  xlim([0 1]) ; xlabel('recall') ;
  ylim([0 1]) ; ylabel('precision') ;
  title(sprintf('PR (AUC: %.2f%%, AP: %.2f%%, AP11: %.2f%%)', ...
                info.auc * 100, ...
                info.ap * 100, ...
                info.ap_interp_11 * 100)) ;
  if opts.interpolate
    legend('PR interp.', 'PR rand.', 'Location', 'SouthEast') ;
  else
    legend('PR', 'PR rand.', 'Location', 'SouthEast') ;
  end
  clear recall precision info ;
end

% --------------------------------------------------------------------
%                                                        Stable output
% --------------------------------------------------------------------

if opts.stable
  precision(1) = [] ;
  recall(1) = [] ;
  precision_ = precision ;
  recall_ = recall ;
  precision = NaN(size(precision)) ;
  recall = NaN(size(recall)) ;
  precision(perm) = precision_ ;
  recall(perm) = recall_ ;
end

% --------------------------------------------------------------------
function h = spline(x,y,spec,varargin)
% --------------------------------------------------------------------
prop = vl_linespec2prop(spec) ;
h = line(x,y,prop{:},varargin{:}) ;
