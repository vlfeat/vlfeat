function x = vl_isoctave()
% VL_ISOCTAVE   Determines whether Octave is running
%   X = VL_ISOCTAVE() returns TRUE if the script is running in the
%   Octave environment (instead of MATLAB).

persistent y ;

if isempty(y)
  y = exist('OCTAVE_VERSION','builtin') ~= 0 ;
end

x = y ;
