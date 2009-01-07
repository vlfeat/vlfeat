% VL_SAMPLINTHIST  Sample integral histogram
%   HISTS = VL_SAMPLINTHIST(INTHIST, BOXES) samples the integral
%   histogram INTHIST to obtain the histograms of the specified
%   BOXES.
%
%   INTHIST is a MxNxK array, where MxN correspond to the ``spatial''
%   dimensions, and K is the number of histogram bins. INTHIST may be
%   of class UINT32 or DOUBLE.
%
%   Each box is a four dimensional vector [XMIN YMIN XMAX YMAX]' of
%   class UINT32 and defines the integer set [XMIN, XMAX] x [YMIN,
%   YMAX]. To specify an empty box, let XMIN = XMAX + 1.
%
%   HISTS is the resulting histogram (one coulm per box) and has K
%   rows, one for each histogram bin. HIST is of the same class of
%   INTHIST.
%
%   See also:: VL_HELP(), VL_INTHIST(), VL_IMINTEGRAL().
