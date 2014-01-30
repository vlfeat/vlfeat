function results = vl_test_binsum(varargin)
% VL_TEST_BINSUM
vl_test_init ;

function test_three_args()
vl_assert_almost_equal(...
  vl_binsum([0 0], 1, 2), [0 1]) ;
vl_assert_almost_equal(...
  vl_binsum([1 7], -1, 1), [0 7]) ;
vl_assert_almost_equal(...
  vl_binsum([1 7], -1, [1 2 2 2 2 2 2 2]), [0 0]) ;

function test_four_args()
vl_assert_almost_equal(...
  vl_binsum(eye(3), [1 1 1],  [1 2 3],  1), 2*eye(3)) ;
vl_assert_almost_equal(...
  vl_binsum(eye(3), [1 1 1]', [1 2 3]', 2), 2*eye(3)) ;
vl_assert_almost_equal(...
  vl_binsum(eye(3), 1, [1 2 3], 1), 2*eye(3)) ;
vl_assert_almost_equal(...
  vl_binsum(eye(3), 1, [1 2 3]', 2), 2*eye(3)) ;

function test_3d_one()
Z = zeros(3,3,3) ;
B = 3*ones(3,1,3) ;
R = Z ; R(:,3,:) = 17 ;
vl_assert_almost_equal(...
  vl_binsum(Z, 17, B, 2), R) ;

function test_3d_two()
Z = zeros(3,3,3) ;
B = 3*ones(3,3,1) ;
X = zeros(3,3,1) ; X(:,:,1) = 17 ;
R = Z ; R(:,:,3) = 17 ;
vl_assert_almost_equal(...
  vl_binsum(Z, X, B, 3), R) ;

function test_storage_classes()
types = {@double, @single, ...
         @int32, @uint32, ...
         @int16, @uint16, ...
         @int8, @uint8} ;
if vl_matlabversion() > 71000
  types = horzcat(types, {@int64, @uint64}) ;
end
for a = types
  a = a{1} ;
  for b = types
    b = b{1} ;
    vl_assert_almost_equal(...
      vl_binsum(a(eye(3)), a([1 1 1]),  b([1 2 3]),  1), a(2*eye(3))) ;
  end
end
