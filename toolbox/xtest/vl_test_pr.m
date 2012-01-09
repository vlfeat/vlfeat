function results = vl_test_pr(varargin)
% VL_TEST_PR
vl_test_init ;

function s = setup()
s.scores0 = [5 4 3 2 1] ;
s.scores1 = [5 3 4 2 1] ;
s.labels = [1 1 -1 -1 -1] ;

function test_perfect_tptn(s)
[rc,pr] = vl_pr(s.labels,s.scores0) ;
vl_assert_almost_equal(pr, [1 1/1 2/2 2/3 2/4 2/5]) ;
vl_assert_almost_equal(rc, [0 1 2 2 2 2] / 2) ;

function test_perfect_metrics(s)
[rc,pr,info] = vl_pr(s.labels,s.scores0) ;
vl_assert_almost_equal(info.auc, 1) ;
vl_assert_almost_equal(info.ap, 1) ;
vl_assert_almost_equal(info.ap_interp_11, 1) ;

function test_swap1_tptn(s)
[rc,pr] = vl_pr(s.labels,s.scores1) ;
vl_assert_almost_equal(pr, [1 1/1 1/2 2/3 2/4 2/5]) ;
vl_assert_almost_equal(rc, [0 1 1 2 2 2] / 2) ;

function test_swap1_tptn_stable(s)
[rc,pr] = vl_pr(s.labels,s.scores1,'stable',true) ;
vl_assert_almost_equal(pr, [1/1 2/3 1/2 2/4 2/5]) ;
vl_assert_almost_equal(rc, [1 2 1 2 2] / 2) ;

function test_swap1_metrics(s)
[rc,pr,info] = vl_pr(s.labels,s.scores1) ;
clf; vl_pr(s.labels,s.scores1) ;
vl_assert_almost_equal(info.auc, [.5 + .5 * (.5 + 2/3)/2]) ;
vl_assert_almost_equal(info.ap, [1/1 + 2/3]/2) ;
vl_assert_almost_equal(info.ap_interp_11, mean([1 1 1 1 1 1 2/3 2/3 2/3 2/3 2/3])) ;
