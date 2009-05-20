% DEMO_ALL  Run all demos

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

vl_demo_mser_basic ;
vl_demo_mser_delta ;
vl_demo_mser_cmd ;

vl_test_hikmeans ;
vl_test_ikmeans ;
vl_test_imintegral ;

vl_demo_aib ;

vl_demo_quickshift ;

rmpath(demo_path);
rmpath(test_path);

