% VL_DEMO  Run all demos

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

root = vl_root ;
demo_path = fullfile(root,'toolbox','demo') ;
test_path = fullfile(root,'toolbox','test') ;

addpath(demo_path) ;
addpath(test_path) ;

vl_demo_sift_basic ;
vl_demo_sift_peak ;
vl_demo_sift_edge ;
vl_demo_sift_cmd ;
vl_demo_sift_or ;
vl_demo_sift_match ;
vl_demo_sift_vs_ubc ;

vl_demo_dsift ;

vl_demo_mser_basic ;
vl_demo_mser_delta ;
vl_demo_mser_cmd ;

vl_test_hikmeans ;
vl_test_ikmeans ;
vl_test_imintegral ;

vl_demo_aib ;

vl_demo_quickshift ;

vl_demo_kdtree ;
vl_demo_kdtree_sift ;
vl_demo_kdtree_self ;
vl_demo_kdtree_forest ;
vl_demo_kdtree_ann ;

vl_demo_imdisttf ;

rmpath(demo_path);
rmpath(test_path);
