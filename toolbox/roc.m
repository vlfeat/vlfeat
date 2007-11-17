function [tp,tn,eer,area] = roc(y, score)
% ROC Compute ROC curve
%
%  [TP,TN] = ROC(Y, SCORE) computes the ROC curve of the specified
%  data. Y are the data labels (+1 or -1) and SCORE is the
%  discriminant score assigned by a classifier (e.g. SVM).  SCORE is
%  supposed to be high when there is high confidence that a sample has
%  positie label.
%
%  [TP,TN] represents the ROC curve.
%
%  [TP,TN,EER,AREA] = ROC(...) return the equal error rate EER and
%  the area under the curve AREA as well.
%
%  The true positive (TP) rate is the fraction of positive samples
%  classified as positive. The false positive (FP) rate is the fracton
%  of negative samples classified as positive. Similarl definitions
%  hold for TN and FN. In formulas:
%
%  TP = Np_good / Np   TN = Nn_good / Nn
%  FP = Nn_bad  / Nn   FN = Np_bad  / Np
%
%  Np = Np_good + Np_bad
%  Nn = Nn_good + Nn_bad
%
%  TN = 1 - FP
%  TP = 1 - FN

% AUTORIGHTS

[drop,perm] = sort(score) ;

N = length(perm) ;

% true positive rate
% fliplr(perm) move the high scored ones to the beginning
tp = cumsum( y(fliplr(perm)) == +1 ) / sum(y==+1) ;
tp = fliplr(tp) ;
tp = [tp 0] ;

% true negative rate
tn = cumsum( y(perm) ~= +1 ) ./ sum(y ~= +1)  ;
tn = [0 tn] ;

% voc style
%[tp, tn] = roc_pascal05(y, score) ;

% equal error rate
i = max(find(tp > tn)) ;
eer = (tp(i) + tn(i)) / 2 ;

% area
area = sum((tp(1:end-1)+tp(2:end)) .* diff(tn))/2 ;

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

