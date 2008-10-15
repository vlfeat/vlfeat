% DEMO_ALL  Run all demos

root = vlfeat_root ;
demo_path = fullfile(root,'toolbox','demo'    ) ;
test_path = fullfile(root,'toolbox','test'    ) ;

addpath(demo_path) ;
addpath(test_path) ;

demo_sift_basic ;
demo_sift_peak ;
demo_sift_edge ;
demo_sift_cmd ;
demo_sift_or ;

demo_mser_basic ;
demo_mser_delta ;
demo_mser_cmd ;

test_hikmeans ;
test_ikmeans ;
test_imintegral ;

demo_aib ;

rmpath(demo_path);
rmpath(test_path);

