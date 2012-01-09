%VL_DEMO_PLOTS_RANK  Demonstates VL_ROC, VL_DET, VL_PR

randn('state',0) ;
numPos = 20 ;
numNeg = 100 ;
labels = [ones(1, numPos) -ones(1,numNeg)] ;
scores = randn(size(labels)) + labels ;

figure(1) ; clf ;
vl_roc(labels,scores) ;
vl_demo_print(1,'plots_rank_roc', 0.5) ;

figure(2) ; clf ;
subplot(2,2,1) ; vl_roc(labels,scores, 'plot', 'tntp') ;
subplot(2,2,2) ; vl_roc(labels,scores, 'plot', 'tptn') ;
subplot(2,2,3) ; vl_roc(labels,scores, 'plot', 'fptp') ;
subplot(2,2,4) ; vl_roc(labels,scores, 'plot', 'fpfn') ;
vl_figaspect(1) ;
vl_demo_print(2,'plots_rank_roc_variants', 1) ;

figure(3) ; clf ;
vl_det(labels,scores) ;
vl_demo_print(3,'plots_rank_det', 0.5) ;

figure(4) ; clf ;
vl_pr(labels,scores) ;
vl_demo_print(4,'plots_rank_pr', 0.5) ;
