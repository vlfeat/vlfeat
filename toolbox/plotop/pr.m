function [recall, precision, info] = pr(y, scores)
% PR Compute precision-recall curve
%  [RECALL, PRECISION] = PR(Y, SCORES) computes the precision-recall
%  (PR) curve of the specified data. Y are the ground thruth labels
%  (+1 or -1) and SCORE is the discriminant score associated to the
%  data by a classifier (lager scores correspond to positive
%  guesses). 
%
%  Remark:: Give -inf score to data which is never retrieved (this
%     will result in maximum recall < 1).
%
%  RECALL and PRECISION are the recall and the precision for
%  increasing values of the decision threshold.
%
%  About the PR curve::
%    We use the same symbols as for the ROC() function. We define the
%    quantities
%
%      P = TP / (TP + FP) = precision
%      R = TP / P = recall
%
%    The precision P is the fraction of positivie predictions which
%    are correct, and the recall R is the fraction of actual positive
%    labels that have been correctly classified (recalled).
%
%    Notice that the recall is also equal to the true positive rate.
%
%  Remark:: precision is undefined (NaN) for those values of the
%    classifier threshold for which no example is classified as
%    positive.
%
%  See also:: ROC(), HELP_VLFEAT().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

[scores, perm] = sort(scores, 'descend') ;
y = y(perm) ;

stop = max(find(scores > -inf)) ;

tp = cumsum(y(1:stop) == +1) ;
fp = cumsum(y(1:stop) == -1) ;
p  = sum(y == +1) ;

recall    = tp / (p + eps) ;
precision = tp ./ (tp + fp + eps) ;

% compute auc
stop = max(find(~isnan(precision))) ;
a    = precision(1:stop) ;
b    = recall(1:stop) ;
auc  = sum((a(1:end-1) + a(2:end)) .* diff(b))/2 ;

% compute auc according to PA08 challenge
ap=0;
for t=0:0.1:1
  p=max(precision(recall>=t));
  if isempty(p)
    p=0;
  end
  ap=ap+p/11;
end

info.auc      = auc ;
info.auc_pa08 = ap ;

% --------------------------------------------------------------------
%                                                                 Plot
% --------------------------------------------------------------------

if nargout == 0	
	cla ; hold on ;
	plot(recall,precision,'linewidth',2) ;
  line([0 1], [1 1] * P / (P + N), 'color', 'r', 'linestyle', '--') ;

	axis square ;
	xlim([0 1]) ; xlabel('recall') ;
	ylim([0 1]) ; ylabel('precision') ;
	title(sprintf('precision-recall (AUC = %.2f %%)', info.auc * 100)) ;
	legend('PR', 'random classifier') ;
end
