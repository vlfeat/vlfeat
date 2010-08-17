function results = vl_test_imarray(varargin)
% VL_TEST_IMARRAY
vl_test_init ;

function test_basic(s)

M = 3 ;
N = 4 ;
width = 32 ;
height = 15 ;

for i=1:M
  for j=1:N
    A{i,j} = rand(width,height) ;
  end
end
A1 = A';
A1 = cat(3,A1{:}) ;
A2 = cell2mat(A) ;
B = vl_imarray(A1, 'layout', [M N]) ;

vl_assert_equal(A2,B) ;
