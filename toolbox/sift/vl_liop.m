% VL_LIOP Local Intensity Order Pattern descriptor
%   D = VL_LIOP(I) computes the LIOP descriptor of an image I, as
%   described by [1]. I is a gray-scale square image with odd side
%   length of class SINGLE. D is a column vector containing the LIOP
%   descriptor of I. Note that LIOP is also integrated in the VL_COVDET()
%   function for feature extraction.
%
%   VL_LIOP() accepts the following options:
%
%   NumNeighbours:: 4
%     Set the number of neighbours sampled to construct the order
%     pattern of each image pixel.
%
%   Radius:: 5
%     Set the radius of the circular neighbourhood used to sampled
%     the local order pattern of each pixel.
%
%   NumSpatialBins:: 6
%     Set the number of spatial pooling regions. The LIOP descriptor
%     has dimension factorial(NumNeighbours) * NumSpatialBins.
%
%   IntensityThreshold:: -0.02
%     Set the intensity threshold used to weight order patterns as they
%     are pooled into a histogram. A negative value is interpreted
%     as a fraction of the difference between the maximum and minimum
%     intensity in each local patch.
%
%   Verbose::
%     If specified, be verbose
%
%   REFERENCES::
%   [1] Z. Wang, B. Fan, F. Wu. Local Intensity Order Pattern for feature
%   description. In ICCV, 2011
%
%   See: <a href="matlab:vl_help('liop')">LIOP</a>, VL_COVDET(),
%   VL_HELP().
