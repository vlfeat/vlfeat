% VL_DEMO_SLICT  Demo: SLIC superpixels

prefix = fullfile(vl_root,'figures','demo') ;
randn('state',0) ;
rand('state',0) ;
figure(1) ; clf ;

% --------------------------------------------------------------------
%                                                        Load a figure
% --------------------------------------------------------------------

im = imread(fullfile(vl_root,'data','roofs1.jpg')) ;
im = im2single(im) ;
im = im(1:128,end-128+1:end,:) ;

figure(1) ; clf ;
image(im) ;
axis equal off tight ;
vl_demo_print('slic_image') ;

% --------------------------------------------------------------------
%                                    Create various SLIC segmentations
% --------------------------------------------------------------------

regionSizes = [10 30] ;
regularizers = [0.01 0.1 1] ;

figure(2) ; clf ;
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

    vl_tightsubplot(numel(regionSizes),numel(regularizers), (i-1)*numel(regularizers) + j) ;
    imagesc(imp) ; axis image off ; hold on ;
    text(5,5,sprintf('regionSize:%.2g\nregularizer:%.2g', regionSize, regularizer), ...
         'Background', 'white','VerticalAlignment','top')
  end
end

vl_demo_print('slic_segmentation') ;
