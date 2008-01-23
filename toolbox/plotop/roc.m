function [tp,tn,info] = roc(y, score)
% ROC Compute ROC curve
%  [TP,TN] = ROC(Y, SCORE) computes the ROC curve of the specified
%  data. Y are the labels (values +1 or -1) and SCORE is the data
%  score, as assigned by some classifier. The function assumes that
%  high value of SCORE predict positive labels and vice-versa.
%
%  [TP,TN] are the true positive and true negative rates for
%  incereasing values of the decision threshold.
%
%  [TP,TN,INFO] = ROC(...) returns the following additional
%  informations:
%
%  INFO.EER       Equal error rate.
%  INFO.AUC       Area under the ROC (AUC).
%  INFO.UR        Uniform prior best op point rate.
%  INFO.UT        Uniform prior best op point threhsold. 
%  INFO.NR        Natural prior best op point rate.
%  INFO.NT        Natural prior best op point threshold.
%
%  ROC(...) plots the ROC diagram in the current axis.
%
%  USING THE ROC CURVE
%
%  We construct a classifer by choosing a threshold t and predicting
%  as poistive the labels with score above the threshold (and negative
%  the others). The ROC curve represent the performance of such
%  classifier as the threshold is changed. Define
%
%    Np,       Nn      = num of pos/neg labels,
%    Np_t(t),  Nn_t(t) = num of pos/neg labels with true prediction,
%    Np_f(t),  Nn_f(t) = num of pos/neg labels with false prediction.
%
%  The performances of the classifier for the threshold t are given by
%  the numbers:
%
%    TP(t) = Np_t(t) / Np = P(ok|pos) = true positive rate
%    TN(t) = Nn_t(t) / Nn = P(ok|neg) = true negative rate
%    FP(t) = Nn_f(t) / Nn = P(e|neg)  = false positive rate
%    FN(t) = Np_f(t) / Np = P(e|pos)  = false negative rate
%
%  (Notice that TN = 1 - FP and TP = 1 - FN so only two numbers are
%  needed.) The ROC curve is the parametric curve (TP(t),TN(t)). The
%  ROC curve corresponding to a random score is the line connecting
%  (1,0) and (0,1).
% 
%  The ROC curve, differently from e.g. the empirical error, does NOT
%  depend on the label prior distribution. In fact, the empirical risk
%  is obtained as
%
%    Remp(t) = FP(t) P(neg) + FN(t) P(pos)
%
%  which requires specifying P(neg) and P(pos).
%
%  An operating point is a point on the ROC curve, corresponding to a
%  certain threshold. An operating point minimizes the empirical error
%  for a certain choice of the labels prior.  In particular, we
%  consider:
%
%  1) NATURAL OPERATING POINT. We assume P(neg) = Nn / N and P(pos) =
%     Np / N as represented by Y.
%  
%  2) UNIFORM OPERATING POINT. We assume P(neg) = P(pos) = 1/2.
%
%  So, forinstance, INFO.UT gives optimal threshold to use in the case
%  of uniform labels prior.

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

[score, perm] = sort(score) ;

Np = sum(y==+1) ;
Nn = sum(y==-1) ;
N  = Np + Nn ;

if N < length(y)
  error('Y must be a vector of -1,+1 values')
end

% true positive rate
% fliplr(perm) move the high scored ones to the beginning
tp = cumsum( y(fliplr(perm)) == +1 ) / Np ;
tp = fliplr(tp) ;
tp = [tp 0] ;

% true negative rate
tn = cumsum(y(perm) == -1) / Nn  ;
tn = [0 tn] ;

% voc style
%[tp, tn] = roc_pascal05(y, score) ;

% --------------------------------------------------------------------
%                                                      Additional info
% --------------------------------------------------------------------

if nargout > 0 | nargout == 0
  % equal error rate
  i1 = max(find(tp >= tn)) ;
  i2 = min(find(tn >= tp)) ;
%  eer = (tp(i) + tn(i)) / 2 ;
  eer = max(tn(i1), tp(i2)) ;
  
  
  % uniform prior and natural prior operating points
  [drop, upoint] = max(tp + tn) ;	
  [drop, npoint] = max(tp .* Np + tn .* Nn) ;	
  
  % uniform prior and natural prior operationg points rates and thresholds
  ur = tp(upoint) * 1/2  + tn(upoint) * 1/2 ;
  nr = tp(npoint) * Np/N + tn(npoint) * Nn/N ;
  ut = score(upoint) ;
  nt = score(npoint) ;
  
  % area
  area = sum((tp(1:end-1)+tp(2:end)) .* diff(tn))/2 ;
  
  % save
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

% plot?
if nargout == 0
	
	% search whre TP + TN is max
	
	cla ; hold on ;
	plot(tp,tn,'linewidth',2) ;
	line(eer             * [0 1 1], ...
			 eer             * [1 1 0], ...
			 'color','r', 'linestyle', '--','linewidth', 1) ;
	line(tp(upoint) * [0 1 1], ...
			 tn(upoint) * [1 1 0], ...
			 'color','g', 'linestyle', '--', 'linewidth', 1) ;
	line(tp(npoint) * [0 1 1], ...
			 tn(npoint) * [1 1 0], ...
			 'color','b', 'linestyle', '--', 'linewidth', 1) ;
	line([0 1], [1 0], 'color','b', 'linestyle', ':', 'linewidth', 2) ;
  line([0 1], [0 1], 'color','y', 'linestyle', '-', 'linewidth', 1) ;

	axis square ;
	xlim([0 1]) ; xlabel('true positve') ;
	ylim([0 1]) ; ylabel('true negative') ;
	title(sprintf('ROC (AUC = %.3g)', area)) ;
	legend('ROC', ...
				 sprintf('eer     %.3g %%', 100 * eer), ...
				 sprintf('op unif %.3g %%', 100 * ur), ...
				 sprintf('op nat  %.3g %%', 100 * nr), ...
				 'random order', 'Location', 'SouthWest') ;
end

% --------------------------------------------------------------------
function [tp,tn] = roc_pascal05(y, score)
% --------------------------------------------------------------------

pres = y > 0 ;
confidence = score ;
n = length(score) ;

% VOC code starts here
rp=randperm(n); % sort equal confidences randomly
pres=pres(rp);
confidence=confidence(rp);

np=sum(pres);
nn=n-np;

[sc,si]=sort(-confidence);
sp=pres(si);
roc.tp=cumsum(sp)/np;
roc.fp=cumsum(~sp)/nn;
% VOC code stops here

tp = fliplr(roc.tp) ;
tn = fliplr(1 - roc.fp) ;
