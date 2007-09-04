function f = ertr(f) ;
% ERTR  Transpose exremal regions frames
%  F = ERTR(F) transposes the frames F as returned by MSER(). This
%  conversion is required as the MSER algorithm considers the column
%  index I as the frist image index, while according standard image
%  convention the first coordinate is the abscissa X.

if size(f,1) ~= 5
  error('F is not in the right format') ;
end

% adjust convention
f = f([2 1 5 4 3],:) ;
