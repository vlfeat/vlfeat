function vl_demo
% VL_DEMO Run VLFeat demos
%   VL_DEMO runs all VLFeat demos in a row, exporting figures for
%   the documentation. Edit this file to see a list of the vailable
%   demos, or look into the toolbox/demo directory.

% Author: Andrea Vedaldi

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

if vl_isoctave()
  more off ;
end

root = vl_root ;
demo_path = fullfile(root,'toolbox','demo') ;
addpath(demo_path) ;

vl_demo_frame ;
vl_demo_covdet ;

vl_demo_sift_basic ;
vl_demo_sift_peak ;
vl_demo_sift_edge ;
%vl_demo_sift_cmd ;
vl_demo_sift_or ;
vl_demo_sift_match ;
vl_demo_sift_vs_ubc ;

vl_demo_mser_basic ;
vl_demo_mser_delta ;
%vl_demo_mser_cmd ;

vl_demo_aib ;
vl_demo_quickshift ;
vl_demo_slic ;
vl_demo_dsift ;
vl_demo_hog ;

vl_demo_svm ;

vl_demo_kdtree ;
vl_demo_kdtree_sift ;
vl_demo_kdtree_self ;
vl_demo_kdtree_forest ;
vl_demo_kdtree_ann ;

vl_demo_kmeans_2d ;

vl_demo_imdisttf ;

vl_demo_plots_rank ;

vl_demo_gmm_2d_rand;
vl_demo_gmm_2d_twist;
vl_demo_gmm_3d;

vl_demo_kmeans_ann_speed;

rmpath(demo_path);
