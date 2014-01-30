function v = vl_matlabversion
% VL_MATLABVERSION  Return MATLAB version as an integer
%   V = VL_MATLABVERSION() returns MATLAB version encoded as an
%   ingeger value. MATLAB version string has the format
%
%     MAJOR.MINOR.SERVICEPACK
%
%   Then V has the value
%
%     V = MAJOR * 1e4 + MINOR + 1e2 + SERVICEPACK.
%
%   For example, in MATLAB 7.10.0 (R2010a), V has the value 7100.
%
%   See also: VL_HELP().

v =  [1e4 1e2 1e0] * sscanf(version, '%d.%d.%d') ;
