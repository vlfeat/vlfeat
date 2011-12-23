% VL_SLIC  SLIC superpixels
%   SEGMENTS = VL_SLIC(IM, REGIONSIZE, REGULARIZER) extracts the SLIC
%   superpixes [1] from image IM. REGIONSIZE is the starting size of
%   the superpixels and REGULARIZER is the trades-off appearance for
%   spatial regularity when clustering (a larger value results in more
%   spatial regularization). SEGMENTS is a UINT32 array containing the
%   superpixel identifier for each image pixel.
%
%   The image IM is a SINGLE array with two or three dimensions. The
%   third dimension is arbitrary, but it is usually three for RGB or
%   LAB images.
%
%   VL_SLIC() accepts the following options:
%
%   Verbose::
%     Be verbose.
%
%   MinRegionSize:: (1/6 of REGIONSIZE)^2
%     The minimum size (in pixel area) of the extracted
%     superpixels.
%
%   Example::
%     If IM contains an RGB images, the SLIC superpixels can be
%     extracted as:
%
%   REFERENCES::
%     [1] R. Achanta, A. Shaji, K. Smith, A. Lucchi, P. Fua, and
%     S. Susstrunk. SLIC superpixels. Technical report, EPFL, 2010.
%
%   See also:: http://www.vlfeat.org/doc/api/slic.html, VL_HELP().
