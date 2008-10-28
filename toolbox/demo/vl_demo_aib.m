function vl_demo_aib
% VL_DEMO_AIB  Test Agglomerative Information Bottleneck (AIB)

D   = 4 ;
K   = 20 ;

randn('state',0) ;
rand('state',0) ;

X1 = randn(2,300) ; X1(1,:) = X1(1,:) + 2 ;
X2 = randn(2,300) ; X2(1,:) = X2(1,:) - 2 ;
X3 = randn(2,300) ; X3(2,:) = X3(2,:) + 2 ;

figure(1) ; clf ; hold on ;
vl_plotframe(X1,'color','r') ;
vl_plotframe(X2,'color','g') ;
vl_plotframe(X3,'color','b') ;
axis equal ;
xlim([-4 4]);
ylim([-4 4]);
axis off ;
rectangle('position',D*[-1 -1 2 2]) 

vl_demo_print('aib_basic_data', .6) ;

C   = 1:K*K ;
Pcx = zeros(3,K*K) ;

f1 = quantize(X1,D,K) ;
f2 = quantize(X2,D,K) ;
f3 = quantize(X3,D,K) ;

Pcx(1,:) = vl_binsum(Pcx(1,:), ones(size(f1)), f1) ;
Pcx(2,:) = vl_binsum(Pcx(2,:), ones(size(f2)), f2) ;
Pcx(3,:) = vl_binsum(Pcx(3,:), ones(size(f3)), f3) ;

Pcx = Pcx / sum(Pcx(:)) ;

[parents, cost] = vl_aib(Pcx) ;

cutsize = [K*K, 10, 3, 2, 1] ;
for i=1:length(cutsize)
  
  [cut,map,short] = vl_aibcut(parents, cutsize(i)) ; 
  parents_cut(short > 0) = parents(short(short > 0)) ;
  C = short(1:K*K+1) ; [drop1,drop2,C] = unique(C) ;
  
  figure(i+1) ; clf ;
  plotquantization(D,K,C) ; hold on ;
  %plottree(D,K,parents_cut) ;
  axis equal ;
  axis off ;
  title(sprintf('%d clusters', cutsize(i))) ;
  
  vl_demo_print(sprintf('aib_basic_clust_%d',i),.6) ;
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

% --------------------------------------------------------------------
function [i,j] = plotquantization(D,K,C)
% --------------------------------------------------------------------
hold on ;
cl = [[.3 .3 .3] ; .5*hsv(max(C)-1)+.5] ;
d  = 2*D / K ;
for i=0:K-1
  for j=0:K-1
    patch(d*(j+[0 1 1 0])-D, ...
          d*(i+[0 0 1 1])-D, ...
          cl(C(j*K+i+1),:)) ;
  end
end
% --------------------------------------------------------------------
function h = plottree(D,K,parents)
% --------------------------------------------------------------------

d  = 2*D / K ;
C = zeros(2,2*K*K-1)+NaN ;
N = zeros(1,2*K*K-1) ;

for i=0:K-1
  for j=0:K-1
    C(:,j*K+i+1) = [d*j-D; d*i-D]+d/2 ;
    N(:,j*K+i+1) = 1 ;
  end
end

for i=1:length(parents)
  p = parents(i) ;  
  if p==0, continue ; end;
  if all(isnan(C(:,i))), continue; end
  if all(isnan(C(:,p)))
    C(:,p) = C(:,i) / N(i) ;
  else
    C(:,p) = C(:,p) + C(:,i) / N(i) ;
  end
  N(p) = N(p) + 1 ;
end

C(1,:) = C(1,:) ./ N ;
C(2,:) = C(2,:) ./ N ;

xt = zeros(3, 2*length(parents)-1)+NaN ;
yt = zeros(3, 2*length(parents)-1)+NaN ;

for i=1:length(parents)
  p = parents(i) ;  
  if p==0, continue ; end;
  xt(1,i) = C(1,i) ; xt(2,i) = C(1,p) ;
  yt(1,i) = C(2,i) ; yt(2,i) = C(2,p) ;
end

h=line(xt(:),yt(:),'linestyle','-','marker','.','linewidth',3) ;
