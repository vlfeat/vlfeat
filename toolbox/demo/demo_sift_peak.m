% DEMO_SIFT_PEAK  Demo: SIFT: peak treshold

randn('state',0) ;
rand('state',0) ;

I = double(rand(100,500) <= .005) ;
I = (ones(100,1) * linspace(0,1,500)) .* I ;
I(:,1) = 0 ; I(:,end) = 0 ;
I(1,:) = 0 ; I(end,:) = 0 ;
I = 2*pi*4^2 * imsmooth(I,4) ;
I = single(255 * I) ;

figure(1) ; clf ;
imagesc(I) ; colormap gray ;
axis equal ;  axis off; axis tight ;
hold on ;

demo_print('sift_peak_0') ;

tpr = [0 10 20 30] ;
for tp=tpr
  f = sift(I, ...
           'PeakThresh',   tp,    ...
           'EdgeThresh',   10000, ...
           'FirstOctave', -1) ;
  
  h = plotframe(f) ; 
  set(h,'color','y','linewidth',1) ;
  
  demo_print(sprintf('sift_peak_%d',find(tp==tpr))) ;  
  delete(h) ;
end
