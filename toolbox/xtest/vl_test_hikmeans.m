function results = vl_test_hikmeans(varargin)
% VL_TEST_IKMEANS
vl_test_init ;

function s = setup()
rand('state',0) ;
s.data = uint8(rand(2,1000) * 255) ;

function test_basic(s)
[tree, assign] = vl_hikmeans(s.data,3,100) ;
assign_ = vl_hikmeanspush(tree, s.data) ;
vl_assert_equal(assign,assign_) ;

function test_elkan(s)
[tree, assign] = vl_hikmeans(s.data,3,100,'method','elkan') ;
assign_ = vl_hikmeanspush(tree, s.data) ;
vl_assert_equal(assign,assign_) ;
