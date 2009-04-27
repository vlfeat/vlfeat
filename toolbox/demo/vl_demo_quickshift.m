% VL_DEMO_QUICKSHIFT  Demo: Quick shift: basic functionality

pfx = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------
I = imread(fullfile(vl_root,'data','a.jpg')) ;

image(I) ;
axis equal off tight ;
vl_demo_print('quickshift_image') ;

% --------------------------------------------------------------------
%                                     Create a quickshift segmentation 
% --------------------------------------------------------------------

ratio = 0.5;
kernelsize = 2;
maxdist = 10;

Iseg = vl_quickseg(I, ratio, kernelsize, maxdist);
image(Iseg);
axis equal off tight;
vl_demo_print('quickshift_qseg_1') ;

kernelsize = 2;
maxdist = 20;
Iseg = vl_quickseg(I, ratio, kernelsize, maxdist);
image(Iseg);
axis equal off tight;
vl_demo_print('quickshift_qseg_2') ;


maxdist = 50;
ndists = 10;
Iedge = vl_quickvis(I, ratio, kernelsize, maxdist, ndists);
imagesc(Iedge); 
axis equal off tight;
colormap gray;
vl_demo_print('quickshift_qvis') ;
