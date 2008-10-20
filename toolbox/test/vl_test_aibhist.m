% VL_TEST_AIBHIST
function vl_test_aibhist
 
D   = 4 ;
K   = 20 ;

randn('state',0) ;
rand('state',0) ;

X1 = randn(2,300) ; X1(1,:) = X1(1,:) + 2 ;
X2 = randn(2,300) ; X2(1,:) = X2(1,:) - 2 ;
X3 = randn(2,300) ; X3(2,:) = X3(2,:) + 2 ;

C   = 1:K*K ;
Pcx = zeros(3,K*K) ;

f1 = quantize(X1,D,K) ;
f2 = quantize(X2,D,K) ;
f3 = quantize(X3,D,K) ;

Pcx(1,:) = vl_binsum(Pcx(1,:), ones(size(f1)), f1) ;
Pcx(2,:) = vl_binsum(Pcx(2,:), ones(size(f2)), f2) ;
Pcx(3,:) = vl_binsum(Pcx(3,:), ones(size(f3)), f3) ;

Pcx = Pcx / sum(Pcx(:)) ;

[parents_, cost_] = vl_aib(Pcx) ;
[parents,  cost ] = vl_aib(Pcx,'clusternull') ;

% find a null node for testing purposes
anull = min(find(parents_==0)) ;
f1 = [f1 repmat(anull,1,10)] ;

figure(100);  clf ;
subplot(1,2,1) ; hold on ;
plot(parents_, 'r.-') ;
plot(parents,  'g') ;
legend('signal null', 'cluster null') ;
subplot(1,2,2) ; hold on ;
plot(cost_, 'r.-') ;
plot(cost,  'g') ;
legend('signal null', 'cluster null') ;

range = [1 10 K*K-10 K*K] ;
for c=1:length(range)
    
  cut_size = range(c) ;
    
  % compare two methods of getting the same cut histogram
  [cut_,map_]  = vl_aibcut(parents_, cut_size) ;
  hist_        = vl_aibcuthist(map_, f1, 'nulls', 'append') ;
  histtree_    = vl_aibhist(parents_, f1) ;
  thist_       = histtree_(cut_) ;
  
  [cut,map]    = vl_aibcut(parents, cut_size) ;
  hist         = vl_aibcuthist(map, f1, 'nulls', 'append') ;
  histtree     = vl_aibhist(parents, f1) ;
  thist        = histtree(cut) ;
    
  figure(100 + c) ; clf ;
  subplot(2,2,1) ; hold on ; plot(hist_,'g.-') ; plot(thist_,'r') ;
  legend('cut+cuthist', 'hist+cut') ;
  title('vl_aibcuthist vs aibhist');
  subplot(2,2,2) ; hold on ; plot(histtree_) ; title('aibtree') ;
  
  subplot(2,2,3) ; hold on ; plot(hist,'g.-') ; plot(thist,'r') ;
  legend('cut+cuthist', 'hist+cut') ;
  title('vl_aibcuthist vs vl_aibhist (clust null)');
  subplot(2,2,4) ; hold on ; plot(histtree) ; title('aibtree (clust null)') ;  
end

% --------------------------------------------------------------------
function f = quantize(X,D,K)
% --------------------------------------------------------------------
d = 2*D / K ;
j = round((X(1,:) + D) / d) ;
i = round((X(2,:) + D) / d) ;
j = max(min(j,K),1) ;
i = max(min(i,K),1) ;
f = sub2ind([K K],i,j) ;
