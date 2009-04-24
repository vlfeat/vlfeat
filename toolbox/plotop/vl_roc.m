function [tp,tn,info] = vl_roc(y, score, varargin)
% VL_ROC Compute the ROC curve
%  [TP,TN] = VL_ROC(Y, SCORE) computes the VL_ROC curve of the specified
%  data. Y are the ground thruth labels (+1 or -1) and SCORE is the
%  discriminant score associated to the data by a classifier (higher
%  scores correspond to positive labels).
%
%  [TP,TN] are the true positive and true negative rates for
%  incereasing values of the decision threshold.
%
%  [TP,TN,INFO] = VL_ROC(...) returns the following additional
%  informations:
%
%  INFO.EER::       Equal error rate.
%  INFO.AUC::       Area under the VL_ROC (AUC).
%  INFO.UR::        Uniform prior best op point rate.
%  INFO.UT::        Uniform prior best op point threhsold. 
%  INFO.NR::        Natural prior best op point rate.
%  INFO.NT::        Natural prior best op point threshold.
%
%  VL_ROC(...) plots the VL_ROC diagram in the current axis.
%
%  About the VL_ROC curve::
%    Consider a classifier that predicts as positive al lables whose
%    SCORE is not smaller than a threshold. The VL_ROC curve represents
%    the performance of such classifier as the threshold r is
%    varied. Denote:
%
%      P  = num of positive samples
%      N  = num of negative samples
%      TP = num of samples that are correctly classified as positive
%      TN = num of samples that are correctly classified as negative
%      FP = num of samples that are incorrectly classified as positive
%      FN = num of samples that are incorrectly classified as negative
%
%    Consider also the rates:
%
%                TP_ = TP / P,      FN_ = FN / P,
%                TN_ = TN / N,      FP_ = FP / N.
%
%    Notice that:
%
%                 P = TP  + FN ,    N = TN  + FP,
%                 1 = TP_ + FN_,    1 = TN_ + FP_.
%
%    The VL_ROC curve is the parametric curve (TP_, TN_) obtained
%    as the classifier threshold is changed.
%
%    The VL_ROC curve is contained in the square with vertices (0,0)
%    The (average) VL_ROC curve of a random classifier is a line which
%    connects (1,0) and (0,1).
% 
%    The VL_ROC curve is independent of the prior probability of positive
%    PPOS and negative labels PNEG. For instance, the empirical
%    expected error (01-risk) is
%
%        ERR = FP_ PPOS + FN_ PNEG,   PPOS = P/(P+N), 
%                                     PNEG = N/(P+N). 
%
%    An OPERATING POINT is a point on the VL_ROC curve, corresponding to
%    a certain threshold. Each operating point minimizes the empirical
%    error for certain label priors PPOS and PPNEG.  VL_ROC() computes
%    the following operating points:
%
%     Natural operating point:: Assumes PPOS = P/(P+N).
%     Uniform operating point:: Assumes PPOS = 1/2.
%
%  See also:: VL_HELP().

% AUTORIGHTS
% Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson
% 
% This file is part of VLFeat, available in the terms of the GNU
% General Public License version 2.

[score, perm] = sort(score) ;

% important for the flipping below
perm = perm(:)' ;

Np = sum(y==+1) ;
Nn = sum(y==-1) ;
N  = Np + Nn ;

if N < length(y)
  error('Y must be a vector of -1 and +1')
end

% true positive rate
% fliplr(perm) move the high scored ones to the beginning
tp = cumsum( y(fliplr(perm)) == +1 ) / (Np + eps) ;
tp = fliplr(tp) ;
tp = [tp 0] ;

% true negative rate
tn = cumsum(y(perm) == -1) / (Nn + eps)  ;
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
  
  score_ = [-inf score] ;
  ut = score_(upoint) ;
  nt = score_(npoint) ;
  
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
		
	cla ; hold on ;
	plot(tn,tp,'linewidth',2) ;

	if ~ opts.falsePositive
    line(eer             * [0 1 1], ...
         eer             * [1 1 0], ...
         'color','r', 'linestyle', '--','linewidth', 1) ;
    line(tn(upoint) * [0 1 1], ...
         tp(upoint) * [1 1 0], ...
         'color','g', 'linestyle', '--', 'linewidth', 1) ;
    line(tn(npoint) * [0 1 1], ...
         tp(npoint) * [1 1 0], ...
         'color','b', 'linestyle', '--', 'linewidth', 1) ;
    line([0 1], [1 0], 'color','b', 'linestyle', ':', 'linewidth', 2) ;
    line([0 1], [0 1], 'color','y', 'linestyle', '-', 'linewidth', 1) ;
    
    xlim([0 1]) ; xlabel('true negative rate') ;
    ylim([0 1]) ; ylabel('true positve rate') ;
  else
    line(1 - eer             * [0 1 1], ...
             eer             * [1 1 0], ...
         'color','r', 'linestyle', '--','linewidth', 1) ;
    line(1 - tn(upoint) * [0 1 1], ...
             tp(upoint) * [1 1 0], ...
         'color','g', 'linestyle', '--', 'linewidth', 1) ;
    line(1 - tn(npoint) * [0 1 1], ...
             tp(npoint) * [1 1 0], ...
         'color','b', 'linestyle', '--', 'linewidth', 1) ;
    line([1 0], [1 0], 'color','b', 'linestyle', ':', 'linewidth', 2) ;
    line([1 0], [0 1], 'color','y', 'linestyle', '-', 'linewidth', 1) ;
    
    xlim([0 1]) ; xlabel('false negative rate') ;
    ylim([0 1]) ; ylabel('true positve rate') ;    
  end
  axis square ;
	title(sprintf('VL_ROC (AUC = %.3g)', area)) ;
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
