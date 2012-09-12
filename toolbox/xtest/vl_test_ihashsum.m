function results = vl_test_ihashsum(varargin)
% VL_TEST_IHASHSUM
vl_test_init ;

function s = setup()
rand('state',0) ;
s.data = uint8(round(16*rand(2,100))) ;
sel = find(all(s.data==0)) ;
s.data(1,sel)=1 ;

function test_hash(s)
D = size(s.data,1) ;
K = 5 ;
h = zeros(1,K,'uint32') ;
id = zeros(D,K,'uint8');
next = zeros(1,K,'uint32') ;
[h,id,next] = vl_ihashsum(h,id,next,K,s.data) ;

sel = vl_ihashfind(id,next,K,s.data) ;
count = double(h(sel)) ;

[drop,i,j] = unique(s.data','rows') ;
for k=1:size(s.data,2)
  count_(k) = sum(j == j(k)) ;
end
vl_assert_equal(count,count_) ;
