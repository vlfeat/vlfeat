classdef vl_test_pr < matlab.unittest.TestCase
  properties
    scores0
    scores1
    labels
  end
  
  methods (TestClassSetup)
    function ssetup(t)
      t.scores0 = [5 4 3 2 1] ;
      t.scores1 = [5 3 4 2 1] ;
      t.labels = [1 1 -1 -1 -1] ;
    end
  end
  
  methods (Test)
    function test_perfect_tptn(t)
      [rc,pr] = vl_pr(t.labels,t.scores0) ;
      t.verifyEqual(pr, [1 1/1 2/2 2/3 2/4 2/5], 'AbsTol', 1e-7) ;
      t.verifyEqual(rc, [0 1 2 2 2 2] / 2, 'AbsTol', 1e-7) ;
    end
    function test_perfect_metrics(t)
      [rc,pr,info] = vl_pr(t.labels,t.scores0) ;
      t.verifyEqual(info.auc, 1, 'AbsTol', 1e-7) ;
      t.verifyEqual(info.ap, 1, 'AbsTol', 1e-7) ;
      t.verifyEqual(info.ap_interp_11, 1, 'AbsTol', 1e-7) ;
    end
    function test_swap1_tptn(t)
      [rc,pr] = vl_pr(t.labels,t.scores1) ;
      t.verifyEqual(pr, [1 1/1 1/2 2/3 2/4 2/5], 'AbsTol', 1e-7) ;
      t.verifyEqual(rc, [0 1 1 2 2 2] / 2, 'AbsTol', 1e-7) ;
    end
    function test_swap1_tptn_stable(t)
      [rc,pr] = vl_pr(t.labels,t.scores1,'stable',true) ;
      t.verifyEqual(pr, [1/1 2/3 1/2 2/4 2/5], 'AbsTol', 1e-7) ;
      t.verifyEqual(rc, [1 2 1 2 2] / 2, 'AbsTol', 1e-7) ;
    end
    function test_swap1_metrics(t)
      [rc,pr,info] = vl_pr(t.labels,t.scores1) ;
      clf; vl_pr(t.labels,t.scores1) ;
      t.verifyEqual(info.auc, [.5 + .5 * (.5 + 2/3)/2], 'AbsTol', 1e-7) ;
      t.verifyEqual(info.ap, [1/1 + 2/3]/2, 'AbsTol', 1e-7) ;
      t.verifyEqual(info.ap_interp_11, mean([1 1 1 1 1 1 2/3 2/3 2/3 2/3 2/3]), ...
        'AbsTol', 1e-7) ;
    end
    function test_inf(t)
      scores = [1 -inf -1 -1 -1 -1] ;
      labels = [1 1    -1 -1 -1 -1] ;
      [rc1,pr1,info1] = vl_pr(labels, scores, 'includeInf', true) ;
      [rc2,pr2,info2] = vl_pr(labels, scores, 'includeInf', false) ;
      
      vl_assert_equal(numel(rc1), numel(rc2) + 1) ;
      
      t.verifyEqual(info1.auc, [1 * .5 + (1/5 + 2/6)/2 * .5], 'AbsTol', 1e-7) ;
      t.verifyEqual(info1.ap,  [1 * .5 + 2/6 * .5], 'AbsTol', 1e-7) ;
      t.verifyEqual(info1.ap_interp_11, [1 * 6/11 + 2/6 * 5/11], 'AbsTol', 1e-7) ;
      
      t.verifyEqual(info2.auc, 0.5, 'AbsTol', 1e-7) ;
      t.verifyEqual(info2.ap,  0.5, 'AbsTol', 1e-7) ;
      t.verifyEqual(info2.ap_interp_11, 1 * 6 / 11, 'AbsTol', 1e-7) ;
    end
    function test_inf_stable(t)
      scores = [-1 -1 -1 -1 -inf +1] ;
      labels = [-1 -1 -1 -1 +1   +1] ;
      [rc1,pr1,info1] = vl_pr(labels, scores, 'includeInf', true, 'stable', true) ;
      [rc2,pr2,info2] = vl_pr(labels, scores, 'includeInf', false, 'stable', true) ;
      [rc1_,pr1_,info1_] = vl_pr(labels, scores, 'includeInf', true, 'stable', false) ;
      [rc2_,pr2_,info2_] = vl_pr(labels, scores, 'includeInf', false, 'stable', false) ;
      
      % stability does not change scores
      t.verifyEqual(info1,info1_) ;
      t.verifyEqual(info2,info2_) ;
      
      % unstable with inf (first point (0,1) is conventional)
      t.verifyEqual(rc1_, [0 .5 .5 .5 .5 .5 1], 'AbsTol', 1e-7)
      t.verifyEqual(pr1_, [1  1  1/2 1/3 1/4 1/5 2/6],'AbsTol', 1e-7)
      
      % unstable without inf
      t.verifyEqual(rc2_, [0 .5 .5 .5 .5 .5], 'AbsTol', 1e-7)
      t.verifyEqual(pr2_, [1  1  1/2 1/3 1/4 1/5], 'AbsTol', 1e-7)
      
      % stable with inf (no conventional point here)
      t.verifyEqual(rc1, [.5  .5  .5  .5  1   .5], 'AbsTol', 1e-7) ;
      t.verifyEqual(pr1, [1/2 1/3 1/4 1/5 2/6  1], 'AbsTol', 1e-7) ;
      
      % stable without inf (no conventional point and -inf are NaN)
      t.verifyEqual(rc2, [.5  .5  .5  .5  NaN .5], 'AbsTol', 1e-7) ;
      t.verifyEqual(pr2, [1/2 1/3 1/4 1/5 NaN  1], 'AbsTol', 1e-7) ;
    end
    function test_normalised_pr(t)
      scores = [+1 +2] ;
      labels = [+1 -1] ;
      [rc1,pr1,info1] = vl_pr(labels,scores) ;
      [rc2,pr2,info2] = vl_pr(labels,scores,'normalizePrior',.5) ;
      t.verifyEqual(pr1, pr2, 'AbsTol', 1e-7) ;
      t.verifyEqual(rc1, rc2, 'AbsTol', 1e-7) ;
      
      scores_ = [+1 +2 +2 +2] ;
      labels_ = [+1 -1 -1 -1] ;
      [rc3,pr3,info3] = vl_pr(labels_,scores_) ;
      [rc4,pr4,info4] = vl_pr(labels,scores,'normalizePrior',1/4) ;
      t.verifyEqual(info3, info4, 'AbsTol', 1e-4) ;
    end
    function test_normalised_pr_corner_cases(t)
      scores = 1:10 ;
      labels = ones(1,10) ;
      [rc1,pr1,info1] = vl_pr(labels,scores) ;
      t.verifyEqual(rc1, (0:10)/10, 'AbsTol', 1e-7) ;
      t.verifyEqual(pr1, ones(1,11), 'AbsTol', 1e-7) ;
      
      scores = 1:10 ;
      labels = zeros(1,10) ;
      [rc2,pr2,info2] = vl_pr(labels,scores) ;
      t.verifyEqual(rc2, zeros(1,11), 'AbsTol', 1e-7) ;
      t.verifyEqual(pr2, ones(1,11), 'AbsTol', 1e-7) ;
    end
  end
end
