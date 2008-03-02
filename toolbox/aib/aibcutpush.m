function y = aibcutpush(map, x)
% AIBCUTPUSH  Quantize based on AIB cut
%  Y = AIBCUTPUSH(MAP, X) maps the data X to elements of the AIB cut
%  specified by MAP.
%
%  The function is equivalent to Y = MAP(X).
%
%  See also HELP_VLFEAT(), AIB().

% AUTORIGHTS

y = map(x) ;
