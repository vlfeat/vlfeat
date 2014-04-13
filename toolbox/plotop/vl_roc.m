function [tpr,tnr,info] = vl_roc(labels, scores, varargin)
%VL_ROC   ROC curve.
%   [TPR,TNR] = VL_ROC(LABELS, SCORES) computes the Receiver Operating
%   Characteristic (ROC) curve [1]. LABELS is a row vector of ground
%   truth labels, greater than zero for a positive sample and smaller
%   than zero for a negative one. SCORES is a row vector of
%   corresponding sample scores, usually obtained from a
%   classifier. The scores induce a ranking of the samples where
%   larger scores should correspond to positive labels.
%
%   Without output arguments, the function plots the ROC graph of the
%   specified data in the current graphical axis.
%
%   Otherwise, the function returns the true positive and true
%   negative rates TPR and TNR. These are vectors of the same size of
%   LABELS and SCORES and are computed as follows. Samples are ranked
%   by decreasing scores, starting from rank 1. TPR(K) and TNR(K) are
%   the true positive and true negative rates when samples of rank
%   smaller or equal to K-1 are predicted to be positive. So for
%   example TPR(3) is the true positive rate when the two samples with
%   largest score are predicted to be positive. Similarly, TPR(1) is
%   the true positive rate when no samples are predicted to be
%   positive, i.e. the constant 0.
%
%   Setting a label to zero ignores the corresponding sample in the
%   calculations, as if the sample was removed from the data. Setting
%   the score of a sample to -INF causes the function to assume that
%   that sample was never retrieved. If there are samples with -INF
%   score, the ROC curve is incomplete as the maximum recall is less
%   than 1.
%
%   [TPR,TNR,INFO] = VL_ROC(...) returns an additional structure INFO
%   with the following fields:
%
%   info.auc:: Area under the ROC curve (AUC).
%     This is the area under the ROC plot, the parametric curve
%     (FPR(S), TPR(S)). The PLOT option can be used to plot variants
%     of this curve, which affects the calculation of a corresponding
%     AUC.
%
%   info.eer:: Equal error rate (EER).
%     The equal error rate is the value of FPR (or FNR) when the ROC
%     curves intersects the line connecting (0,0) to (1,1).
%
%   VL_ROC() accepts the following options:
%
%   Plot:: []
%     Setting this option turns on plotting unconditionally. The
%     following plot variants are supported:
%
%     tntp:: Plot TPR against TNR (standard ROC plot).
%     tptn:: Plot TNR against TPR (recall on the horizontal axis).
%     fptp:: Plot TPR against FPR.
%     fpfn:: Plot FNR against FPR (similar to a DET curve).
%
%     Note that this option will affect the INFO.AUC value computation
%     too.
%
%   NumPositives:: []
%   NumNegatives:: []
%     If either of these parameters is set to a number, the function
%     pretends that LABELS contains the specified number of
%     positive/negative labels. NUMPOSITIVES/NUMNEGATIVES cannot be
%     smaller than the actual number of positive/negative entries in
%     LABELS. The additional positive/negative labels are appended to
%     the end of the sequence as if they had -INF scores (as explained
%     above, the function interprets such samples as `not
%     retrieved'). This feature can be used to evaluate the
%     performance of a large-scale retrieval experiment in which only
%     a subset of highly-scoring results are recorded for efficiency
%     reason.
%
%   About the ROC curve::
%     Consider a classifier that predicts as positive all samples whose
%     score is not smaller than a threshold S. The ROC curve represents
%     the performance of such classifier as the threshold S is
%     changed. Formally, define
%
%       P = overall num. of positive samples,
%       N = overall num. of negative samples,
%
%     and for each threshold S
%
%       TP(S) = num. of samples that are correctly classified as positive,
%       TN(S) = num. of samples that are correctly classified as negative,
%       FP(S) = num. of samples that are incorrectly classified as positive,
%       FN(S) = num. of samples that are incorrectly classified as negative.
%
%     Consider also the rates:
%
%       TPR = TP(S) / P,      FNR = FN(S) / P,
%       TNR = TN(S) / N,      FPR = FP(S) / N,
%
%     and notice that, by definition,
%
%       P = TP(S) + FN(S) ,    N = TN(S) + FP(S),
%       1 = TPR(S) + FNR(S),   1 = TNR(S) + FPR(S).
%
%     The ROC curve is the parametric curve (FPR(S), TPR(S)) obtained
%     as the classifier threshold S is varied in the reals. The TPR is
%     the same as `recall' in a PR curve (see VL_PR()).
%
%     The ROC curve is contained in the square with vertices (0,0) The
%     (average) ROC curve of a random classifier is a line which
%     connects (1,0) and (0,1).
%
%     The ROC curve is independent of the prior probability of the
%     labels (i.e. of P/(P+N) and N/(P+N)).
%
%   REFERENCES:
%   [1] http://en.wikipedia.org/wiki/Receiver_operating_characteristic
%
%   See also: VL_PR(), VL_DET(), VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

[tp, fp, p, n, perm, varargin] = vl_tpfp(labels, scores, varargin{:}) ;
opts.plot = [] ;
opts.stable = false ;
opts = vl_argparse(opts,varargin) ;

% compute the rates
small = 1e-10 ;
tpr = tp / max(p, small) ;
fpr = fp / max(n, small) ;
fnr = 1 - tpr ;
tnr = 1 - fpr ;

do_plots = ~isempty(opts.plot) || nargout == 0 ;
if isempty(opts.plot), opts.plot = 'fptp' ; end

% --------------------------------------------------------------------
%                                                      Additional info
% --------------------------------------------------------------------

if nargout > 2 || do_plots
  % Area under the curve. Since the curve is a staircase (in the
  % sense that for each sample either tn is decremented by one
  % or tp is incremented by one but the other remains fixed),
  % the integral is particularly simple and exact.

  switch opts.plot
    case 'tntp', info.auc = -sum(tpr .* diff([0 tnr])) ;
    case 'fptp', info.auc = +sum(tpr .* diff([0 fpr])) ;
    case 'tptn', info.auc = +sum(tnr .* diff([0 tpr])) ;
    case 'fpfn', info.auc = +sum(fnr .* diff([0 fpr])) ;
    otherwise
      error('''%s'' is not a valid PLOT type.', opts.plot);
  end

  % Equal error rate. One must find the index S in correspondence of
  % which TNR(S) and TPR(s) cross. Note that TPR(S) is non-decreasing,
  % TNR(S) is non-increasing, and from rank S to rank S+1 only one of
  % the two quantities can change. Hence there are exactly two types
  % of crossing points:
  %
  %  1) TNR(S) = TNR(S+1) = EER and TPR(S) <= EER, TPR(S+1) > EER,
  %  2) TPR(S) = TPR(S+1) = EER and TNR(S) > EER, TNR(S+1) <= EER.
  %
  % Moreover, if the maximum TPR is smaller than 1, then it is
  % possible that neither of the two cases realizes. In the latter
  % case, we return EER=NaN.

  s = max(find(tnr > tpr)) ;
  if s == length(tpr)
    info.eer = NaN ;
  else
    if tpr(s) == tpr(s+1)
      info.eer = 1 - tpr(s) ;
    else
      info.eer = 1 - tnr(s) ;
    end
  end
end

% --------------------------------------------------------------------
%                                                                 Plot
% --------------------------------------------------------------------

if do_plots
  cla ; hold on ;
  switch lower(opts.plot)
    case 'tntp'
      hroc = plot(tnr, tpr, 'b', 'linewidth', 2) ;
      hrand = spline([0 1], [1 0], 'r--', 'linewidth', 2) ;
      spline([0 1], [0 1], 'k--', 'linewidth', 1) ;
      plot(1-info.eer, 1-info.eer, 'k*', 'linewidth', 1) ;
      xlabel('true negative rate') ;
      ylabel('true positive rate (recall)') ;
      loc = 'sw' ;

    case 'fptp'
      hroc = plot(fpr, tpr, 'b', 'linewidth', 2) ;
      hrand = spline([0 1], [0 1], 'r--', 'linewidth', 2) ;
      spline([1 0], [0 1], 'k--', 'linewidth', 1) ;
      plot(info.eer, 1-info.eer, 'k*', 'linewidth', 1) ;
      xlabel('false positive rate') ;
      ylabel('true positive rate (recall)') ;
      loc = 'se' ;

    case 'tptn'
      hroc = plot(tpr, tnr, 'b', 'linewidth', 2) ;
      hrand = spline([0 1], [1 0], 'r--', 'linewidth', 2) ;
      spline([0 1], [0 1], 'k--', 'linewidth', 1) ;
      plot(1-info.eer, 1-info.eer, 'k*', 'linewidth', 1) ;
      xlabel('true positive rate (recall)') ;
      ylabel('false positive rate') ;
      loc = 'sw' ;

    case 'fpfn'
      hroc = plot(fpr, fnr, 'b', 'linewidth', 2) ;
      hrand = spline([0 1], [1 0], 'r--', 'linewidth', 2) ;
      spline([0 1], [0 1], 'k--', 'linewidth', 1) ;
      plot(info.eer, info.eer, 'k*', 'linewidth', 1) ;
      xlabel('false positive (false alarm) rate') ;
      ylabel('false negative (miss) rate') ;
      loc = 'ne' ;

    otherwise
      error('''%s'' is not a valid PLOT type.', opts.plot);
  end

  grid on ;
  xlim([0 1]) ;
  ylim([0 1]) ;
  axis square ;
  title(sprintf('ROC (AUC: %.2f%%, EER: %.2f%%)', info.auc * 100, info.eer * 100), ...
        'interpreter', 'none') ;
  legend([hroc hrand], 'ROC', 'ROC rand.', 'location', loc) ;
end

% --------------------------------------------------------------------
%                                                        Stable output
% --------------------------------------------------------------------

if opts.stable
  tpr(1) = [] ;
  tnr(1) = [] ;
  tpr_ = tpr ;
  tnr_ = tnr ;
  tpr = NaN(size(tpr)) ;
  tnr = NaN(size(tnr)) ;
  tpr(perm) = tpr_ ;
  tnr(perm) = tnr_ ;
end

% --------------------------------------------------------------------
function h = spline(x,y,spec,varargin)
% --------------------------------------------------------------------
prop = vl_linespec2prop(spec) ;
h = line(x,y,prop{:},varargin{:}) ;
