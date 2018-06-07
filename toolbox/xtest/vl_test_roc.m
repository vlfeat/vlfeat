classdef vl_test_roc < matlab.unittest.TestCase
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
      [tpr,tnr] = vl_roc(t.labels,t.scores0) ;
      t.verifyEqual(tpr, [0 1 2 2 2 2] / 2, 'AbsTol', 1e-7) ;
      t.verifyEqual(tnr, [3 3 3 2 1 0] / 3, 'AbsTol', 1e-7) ;
    end
    function test_perfect_metrics(t)
      [tpr,tnr,info] = vl_roc(t.labels,t.scores0) ;
      t.verifyEqual(info.eer, 0, 'AbsTol', 1e-7) ;
      t.verifyEqual(info.auc, 1, 'AbsTol', 1e-7) ;
    end
    function test_swap1_tptn(t)
      [tpr,tnr] = vl_roc(t.labels,t.scores1) ;
      t.verifyEqual(tpr, [0 1 1 2 2 2] / 2, 'AbsTol', 1e-7) ;
      t.verifyEqual(tnr, [3 3 2 2 1 0] / 3, 'AbsTol', 1e-7) ;
    end
    function test_swap1_tptn_stable(t)
      [tpr,tnr] = vl_roc(t.labels,t.scores1,'stable',true) ;
      t.verifyEqual(tpr, [1 2 1 2 2] / 2, 'AbsTol', 1e-7) ;
      t.verifyEqual(tnr, [3 2 2 1 0] / 3, 'AbsTol', 1e-7) ;
    end
    function test_swap1_metrics(t)
      [tpr,tnr,info] = vl_roc(t.labels,t.scores1) ;
      t.verifyEqual(info.eer, 1/3, 'AbsTol', 1e-7) ;
      t.verifyEqual(info.auc, 1 - 1/(2*3), 'AbsTol', 1e-7) ;
    end
  end
end






