% VL_DEMO_SLICT  Demo: SLIC superpixels

prefix = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------

im = imread(fullfile(vl_root,'data','a.jpg')) ;
im = im2single(im) ;
im = im(1:128,end-128+1:end,:) ;

image(im) ;
axis equal off tight ;
vl_demo_print('slic_image') ;

% --------------------------------------------------------------------
%                                    Create various SLIC segmentations
% --------------------------------------------------------------------

regionSizes = [10 30] ;
regularizers = [0.01 0.1 1] ;

mosaic = {} ;
for i = 1:numel(regionSizes)
  for j = 1:numel(regularizers)
    regionSize = regionSizes(i) ;
    regularizer = regularizers(j) ;
    segments = vl_slic(im, regionSize, regularizer, 'verbose') ;

    % overaly segmentation
    [sx,sy]=vl_grad(double(segments), 'type', 'forward') ;
    s = find(sx | sy) ;
    imp = im ;
    imp([s s+numel(im(:,:,1)) s+2*numel(im(:,:,1))]) = 0 ;

    mosaic{i,j} = imp ;
  end
end

mosaic = cell2mat(mosaic) ;

image(mosaic) ;
axis equal off tight ;
vl_demo_print('slic_segmentation') ;
