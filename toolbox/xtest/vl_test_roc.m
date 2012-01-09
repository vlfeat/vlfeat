function results = vl_test_roc(varargin)
% VL_TEST_ROC
vl_test_init ;

function s = setup()
s.scores0 = [5 4 3 2 1] ;
s.scores1 = [5 3 4 2 1] ;
s.labels = [1 1 -1 -1 -1] ;

function test_perfect_tptn(s)
[tpr,tnr] = vl_roc(s.labels,s.scores0) ;
vl_assert_almost_equal(tpr, [0 1 2 2 2 2] / 2) ;
vl_assert_almost_equal(tnr, [3 3 3 2 1 0] / 3) ;

function test_perfect_metrics(s)
[tpr,tnr,info] = vl_roc(s.labels,s.scores0) ;
vl_assert_almost_equal(info.eer, 0) ;
vl_assert_almost_equal(info.auc, 1) ;

function test_swap1_tptn(s)
[tpr,tnr] = vl_roc(s.labels,s.scores1) ;
vl_assert_almost_equal(tpr, [0 1 1 2 2 2] / 2) ;
vl_assert_almost_equal(tnr, [3 3 2 2 1 0] / 3) ;

function test_swap1_tptn_stable(s)
[tpr,tnr] = vl_roc(s.labels,s.scores1,'stable',true) ;
vl_assert_almost_equal(tpr, [1 2 1 2 2] / 2) ;
vl_assert_almost_equal(tnr, [3 2 2 1 0] / 3) ;

function test_swap1_metrics(s)
[tpr,tnr,info] = vl_roc(s.labels,s.scores1) ;
vl_assert_almost_equal(info.eer, 1/3) ;
vl_assert_almost_equal(info.auc, 1 - 1/(2*3)) ;


