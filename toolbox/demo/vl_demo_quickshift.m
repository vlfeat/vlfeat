% VL_DEMO_MSER_BASIC  Demo: MSER: basic functionality

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

ratio = 1;
sigma = 2;
tau = 10;

Iseg = vl_quickseg(I, ratio, sigma, tau);
image(Iseg);
axis equal off tight;
vl_demo_print('quickshift_qseg_1') ;

sigma = 2;
tau = 30;
Iseg = vl_quickseg(I, ratio, sigma, tau);
image(Iseg);
axis equal off tight;
vl_demo_print('quickshift_qseg_2') ;


tau_max = 50;
ntaus = 5;
Iedge = vl_quickvis(I, ratio, sigma, tau_max, ntaus);
imagesc(Iedge); 
axis equal off tight;
vl_demo_print('quickshift_qvis') ;
