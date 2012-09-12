function results = vl_test_ikmeans(varargin)
% VL_TEST_IKMEANS
vl_test_init ;

function s = setup()
rand('state',0) ;
s.data = uint8(rand(2,1000) * 255) ;

function test_basic(s)
[centers, assign] = vl_ikmeans(s.data,100) ;
assign_ = vl_ikmeanspush(s.data, centers) ;
vl_assert_equal(assign,assign_) ;

function test_elkan(s)
[centers, assign] = vl_ikmeans(s.data,100,'method','elkan') ;
assign_ = vl_ikmeanspush(s.data, centers) ;
vl_assert_equal(assign,assign_) ;
