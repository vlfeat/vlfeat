function f = vl_ertr(f) ;
% VL_ERTR  Transpose exremal regions frames
%  F = VL_ERTR(F) transposes the frames F as returned by VL_MSER(). This
%  conversion is required as the VL_MSER algorithm considers the column
%  index I as the frist image index, while according standard image
%  convention the first coordinate is the abscissa X.
%
%  See also VL_HELP(), VL_MSER().

if size(f,1) ~= 5
  error('F is not in the right format') ;
end

% adjust convention
f = f([2 1 5 4 3],:) ;
