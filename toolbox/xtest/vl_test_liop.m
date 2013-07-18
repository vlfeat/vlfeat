function results = vl_test_liop(varargin)
% VL_TEST_SIFT
vl_test_init ;

function s = setup()
randn('state',0) ;
s.patch = randn(65,'single') ;
xr = -32:32 ;
[x,y] = meshgrid(xr) ;
s.blob = - single(x.^2+y.^2) ;

function test_basic(s)
d = vl_liop(s.patch) ;

function test_blob(s)
% with a blob, all local intensity order pattern are equal. In
% particular, if the blob intensity decreases away from the center,
% then all local intensities sampled in a neighbourhood of 2 elements
% are already sorted (see LIOP details).
d = vl_liop(s.blob, ...
            'IntensityThreshold', 0,  ...
            'NumNeighbours', 2, ...
            'NumSpatialBins', 1) ;
assert(isequal(d, single([1;0]))) ;

function test_neighbours(s)
for n=2:5
  for p=1:3
    d = vl_liop(s.patch, 'NumNeighbours', n, 'NumSpatialBins', p) ;
    assert(numel(d) == p * factorial(n)) ;
  end
end

function test_multiple(s)
x = randn(31,31,3, 'single') ;
d = vl_liop(x) ;
for i=1:3
  d_(:,i) = vl_liop(squeeze(x(:,:,i))) ;
end
assert(isequal(d,d_)) ;
