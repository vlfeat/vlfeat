function results = vl_test_imintegral(varargin)
% VL_TEST_IMINTEGRAL
vl_test_init ;

function state = setup()
state.I = ones(5,6) ;
state.correct = [ 1     2     3     4     5     6 ;
                  2     4     6     8    10    12 ;
                  3     6     9    12    15    18 ;
                  4     8    12    16    20    24 ;
                  5    10    15    20    25    30 ; ] ;

function test_matlab_equivalent(s)
vl_assert_equal(slow_imintegral(s.I), s.correct) ;

function test_basic(s)
vl_assert_equal(vl_imintegral(s.I), s.correct) ;

function test_multi_dimensional(s)
vl_assert_equal(vl_imintegral(repmat(s.I, [1 1 3])), ...
                repmat(s.correct, [1 1 3])) ;

function test_random(s)
numTests = 50 ;
for i = 1:numTests
    I = rand(5) ;
    vl_assert_almost_equal(vl_imintegral(s.I), ...
                           slow_imintegral(s.I)) ;
end

function test_datatypes(s)
vl_assert_equal(single(vl_imintegral(s.I)), single(s.correct)) ;
vl_assert_equal(double(vl_imintegral(s.I)), double(s.correct)) ;
vl_assert_equal(uint32(vl_imintegral(s.I)), uint32(s.correct)) ;
vl_assert_equal(int32(vl_imintegral(s.I)), int32(s.correct)) ;
vl_assert_equal(int32(vl_imintegral(-s.I)), -int32(s.correct)) ;

function integral = slow_imintegral(I)
integral = zeros(size(I));
for k = 1:size(I,3)
  for r = 1:size(I,1)
    for c = 1:size(I,2)
      integral(r,c,k) = sum(sum(I(1:r,1:c,k)));
    end
  end
end
