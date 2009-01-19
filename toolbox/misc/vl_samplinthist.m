% VL_SAMPLINTHIST  Sample integral histogram
%   HISTS = VL_SAMPLINTHIST(INTHIST, BOXES) samples the integral
%   histogram INTHIST to obtain the histograms of the specified
%   BOXES.
%
%   INTHIST is a MxNxK array, where M x N are ``spatial'' dimensions,
%   and K is the number of histogram bins. INTHIST may be of class
%   UINT32 or DOUBLE.
%
%   Each box is a four dimensional vector [IMIN JMIN IMAX JMAX]' of
%   class UINT32 and correspond to the index set [IMIN, IMAX] x [JMIN,
%   JMAX]. To specify an empty box, let IMIN > IMAX.
%
%   HISTS stores one histogram per column (one for each box) and has K
%   rows, one for each histogram bin. HIST is of the same class of
%   INTHIST.
%
%   See also:: VL_HELP(), VL_INTHIST(), VL_IMINTEGRAL().
