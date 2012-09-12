function results = vl_test_inthist(varargin)
% VL_TEST_INTHIST
vl_test_init ;

function s = setup()
rand('state',0) ;
s.labels = uint32(8*rand(123, 76, 3)) ;

function test_basic(s)
l = 10 ;
hist = vl_inthist(s.labels, 'numlabels', l) ;
hist_ = inthist_slow(s.labels, l) ;
vl_assert_equal(double(hist),hist_) ;

function test_sample(s)
rand('state',0) ;
boxes = 10 * rand(4,20) + .5 ;
boxes(3:4,:) = boxes(3:4,:) + boxes(1:2,:) ;
boxes = min(boxes, 10) ;
boxes = uint32(boxes) ;
inthist = vl_inthist(s.labels) ;
hist = vl_sampleinthist(inthist, boxes) ;

function hist = inthist_slow(labels, numLabels)
m = size(labels,1) ;
n = size(labels,2) ;
l = numLabels ;
b = zeros(m*n,l) ;
b = vl_binsum(b, 1, reshape(labels,m*n,[]), 2) ;
b = reshape(b,m,n,l) ;
for k=1:l
  hist(:,:,k) = cumsum(cumsum(b(:,:,k)')') ;
end
