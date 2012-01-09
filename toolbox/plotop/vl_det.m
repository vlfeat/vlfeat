function [missRate, falseAlarm] = vl_det(labels, scores, varargin)
% VL_DET  Compute DET curve
%  [MISSRATE, FALSEALRAM] = VL_DET(LABELS, SCORES) computes the
%  Detection Error Trade-off (DET) curve for the given set LABELS and
%  SCORES. The function is a simple wrapper to VL_ROC(LABELS,
%  SCORES). In fact, MISSRATE is simply the false positive rate and
%  FALSEARALM the false negative rate returned by VL_ROC().
%
%  VL_DET(...) without output arguments plots the curves in the
%  current axis. Note that the DET curves use logarithmic axis.
%
%  VL_DET(..., OPT, ARG, ...) accepts the same options as VL_ROC().
%
%  REFERENCES:
%  [1] A. Martin, G. Doddington, T. Kamm, M. Ordowski, and
%  M. Przybocki. The DET curve in assessment of detection task
%  performance. In Proc. Conf. on Speech Communication and Technology,
%  1997.

[tp,tn] = vl_roc(labels, scores, varargin{:}) ;
missRate = 1 - tp ;
falseAlarm = 1 - tn ;

if nargout == 0
  cla ;
  loglog(falseAlarm,missRate,'linewidth',2) ;
  grid on ;
  axis square ;
  xlabel('false alaram (false positive) rate') ;
  ylabel('miss (false negative) rate') ;
  title('DET') ;
end

% %conf = vl_argparse(conf, varargin{:}) ;

% for i = 1:2:length(varargin)
%   opt = varargin{i} ;
%   val = varargin{i+1} ;
%   switch lower(opt)
%     case 'numnegatives'
%       N = val ;
%     otherwise
%       error('Unknown option %s', opt) ;
%   end
% end

% P = sum(labels > 0) ;
% if ~exist('N'), N = sum(labels < 0) ; end

% if N < sum(labels < 0), warning('NumNegatives < number of provided negatives') ; end

% N_=N-sum(labels<0) ;
% [scores, perm] = sort(scores) ;

% disp(sum(labels<0)/N)

% tp = [0 cumsum(labels(fliplr(perm)) > 0)/P 1] ;
% tn = [1 (fliplr(cumsum(labels(perm) < 0))+N_)/N 0] ;

