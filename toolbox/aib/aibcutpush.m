function y = aibcutpush(map, x)
% AIBCUTPUSH
%  Y = AIBCUTPUSH(MAP, X) relabels the data X as elements of the
%  AIB cut specified by MAP (as returned by AIBCUT()).
%
%  The function is equivalent to Y = MAP(X).

% AUTORIGHTS

y = map(x) ;
