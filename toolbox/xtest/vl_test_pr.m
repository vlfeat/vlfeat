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

function test_inf(s)
scores = [1 -inf -1 -1 -1 -1] ;
labels = [1 1    -1 -1 -1 -1] ;
[rc1,pr1,info1] = vl_pr(labels, scores, 'includeInf', true) ;
[rc2,pr2,info2] = vl_pr(labels, scores, 'includeInf', false) ;

vl_assert_equal(numel(rc1), numel(rc2) + 1) ;

vl_assert_almost_equal(info1.auc, [1 * .5 + (1/5 + 2/6)/2 * .5]) ;
vl_assert_almost_equal(info1.ap,  [1 * .5 + 2/6 * .5]) ;
vl_assert_almost_equal(info1.ap_interp_11, [1 * 6/11 + 2/6 * 5/11]) ;

vl_assert_almost_equal(info2.auc, 0.5) ;
vl_assert_almost_equal(info2.ap,  0.5) ;
vl_assert_almost_equal(info2.ap_interp_11, 1 * 6 / 11) ;

function test_inf_stable(s)
scores = [-1 -1 -1 -1 -inf +1] ;
labels = [-1 -1 -1 -1 +1   +1] ;
[rc1,pr1,info1] = vl_pr(labels, scores, 'includeInf', true, 'stable', true) ;
[rc2,pr2,info2] = vl_pr(labels, scores, 'includeInf', false, 'stable', true) ;
[rc1_,pr1_,info1_] = vl_pr(labels, scores, 'includeInf', true, 'stable', false) ;
[rc2_,pr2_,info2_] = vl_pr(labels, scores, 'includeInf', false, 'stable', false) ;

% stability does not change scores
vl_assert_almost_equal(info1,info1_) ;
vl_assert_almost_equal(info2,info2_) ;

% unstable with inf (first point (0,1) is conventional)
vl_assert_almost_equal(rc1_, [0 .5 .5 .5 .5 .5 1])
vl_assert_almost_equal(pr1_, [1  1  1/2 1/3 1/4 1/5 2/6])

% unstable without inf
vl_assert_almost_equal(rc2_, [0 .5 .5 .5 .5 .5])
vl_assert_almost_equal(pr2_, [1  1  1/2 1/3 1/4 1/5])

% stable with inf (no conventional point here)
vl_assert_almost_equal(rc1, [.5  .5  .5  .5  1   .5]) ;
vl_assert_almost_equal(pr1, [1/2 1/3 1/4 1/5 2/6  1]) ;

% stable without inf (no conventional point and -inf are NaN)
vl_assert_almost_equal(rc2, [.5  .5  .5  .5  NaN .5]) ;
vl_assert_almost_equal(pr2, [1/2 1/3 1/4 1/5 NaN  1]) ;

function test_normalised_pr(s)
scores = [+1 +2] ;
labels = [+1 -1] ;
[rc1,pr1,info1] = vl_pr(labels,scores) ;
[rc2,pr2,info2] = vl_pr(labels,scores,'normalizePrior',.5) ;
vl_assert_almost_equal(pr1, pr2) ;
vl_assert_almost_equal(rc1, rc2) ;

scores_ = [+1 +2 +2 +2] ;
labels_ = [+1 -1 -1 -1] ;
[rc3,pr3,info3] = vl_pr(labels_,scores_) ;
[rc4,pr4,info4] = vl_pr(labels,scores,'normalizePrior',1/4) ;
vl_assert_almost_equal(info3, info4) ;

function test_normalised_pr_corner_cases(s)
scores = 1:10 ;
labels = ones(1,10) ;
[rc1,pr1,info1] = vl_pr(labels,scores) ;
vl_assert_almost_equal(rc1, (0:10)/10) ;
vl_assert_almost_equal(pr1, ones(1,11)) ;

scores = 1:10 ;
labels = zeros(1,10) ;
[rc2,pr2,info2] = vl_pr(labels,scores) ;
vl_assert_almost_equal(rc2, zeros(1,11)) ;
vl_assert_almost_equal(pr2, ones(1,11)) ;

