function [recall, precision, info] = vl_pr(y, scores)
% VL_PR Compute precision-recall curve
%  [RECALL, PRECISION] = VL_PR(Y, SCORES) computes the precision-recall
%  (PR) curve of the specified data. Y are the ground thruth labels
%  (+1 or -1) and SCORE is the discriminant score associated to the
%  data by a classifier (lager scores correspond to positive
%  guesses). 
%
%  Remark:: Assign -INF score to data which is never retrieved
%    (this will result in maximum recall < 1).
%
%  RECALL and PRECISION are the recall and the precision for
%  increasing values of the decision threshold.
%
%  About the PR curve::
%    We use the same symbols as for the VL_ROC() function. We define the
%    quantities
%
%      P = TP / (TP + FP) = precision
%      R = TP / P = recall
%
%    The precision P is the fraction of positivie predictions which
%    are correct, and the recall R is the fraction of trurly positive
%    labels that have been correctly classified (recalled).
%
%    Notice that the recall is also equal to the true positive rate in
%    a ROC curve (see VL_ROC()).
%
%  Remark:: precision (P) is undefined for those values of the
%    classifier threshold for which no example is classified as
%    positive. Conventionally, we assign a precision of P=1 to such
%    cases.
%
%  See also:: VL_ROC(), VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

[scores, perm] = sort(scores, 'descend') ;
y = y(perm) ;

stop = max(find(scores > -inf)) ;

tp = [0 cumsum(y(1:stop) == +1)] ;
fp = [0 cumsum(y(1:stop) == -1)] ;
p  = sum(y == +1) ;

recall    = tp  / (p + eps) ;
precision = (tp + eps) ./ (tp + fp + eps) ;

% compute auc
if length(recall) > 1
  a    = precision ;
  b    = recall ;
  auc  = sum((a(1:end-1) + a(2:end)) .* diff(b))/2 ;
else
  auc  = 0 ;
end

% compute auc according to PA08 challenge
ap=0;
for t=0:0.1:1
  p_=max(precision(recall>=t));
  if isempty(p_)
    p_=0;
  end
  ap=ap+p_/11;
end

info.auc      = auc ;
info.auc_pa08 = ap ;

% --------------------------------------------------------------------
%                                                                 Plot
% --------------------------------------------------------------------

if nargout == 0	
	cla ; hold on ;
	plot(recall,precision,'linewidth',2) ;
  line([0 1], [1 1] * p / length(y), 'color', 'r', 'linestyle', '--') ;

	axis square ;
	xlim([0 1]) ; xlabel('recall') ;
	ylim([0 1]) ; ylabel('precision') ;
	title(sprintf('precision-recall (AUC = %.2f %%)', info.auc * 100)) ;
	legend('PR', 'random classifier', 'location', 'northwestoutside') ;

  clear recall precision info ;
end
