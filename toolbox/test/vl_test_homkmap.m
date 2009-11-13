function vl_test_homkmap

x = 2.^(-12:.1:0) ;
L = .3 ;
n = 4 ;

V = vl_homkmap(x, n, L, 'kchi2') ;

V_ = featureMap('chi2', n, L, x, 1) ;

V
V_

figure(1) ; clf ;
subplot(1,2,1) ;
semilogx(x,V_','-') ; hold on ;
semilogy(x,V','--')  ;
subplot(1,2,2); 
plot(x,V_','-') ; hold on ;
plot(x,V','--')  ;



function psi = featureMap(kappa, n, L, x, g)

if nargin < 5, g = 1 ; end

if isstr(kappa)
  switch kappa
    case 'inters'
      kappa = @(lambda) 2/pi * 1 ./ (1 + 4 * lambda.^2) ;
    case 'chi2'
      kappa = @(lambda) sech(pi * lambda) ;
    otherwise
      error('Unknown kernel') ;
  end
end

l = (1:n) * L ;
skp0 = sqrt(L)   * sqrt(kappa(0)) ;
s2kp = sqrt(2*L) * sqrt(kappa(l)) ;

[d, M] = size(x) ;
sz = 1 + 2*n ;
psi = zeros(d * sz, M) ;

% do this in blocks to avoid using too much memory
br = [1:ceil(2e6 / d):M, M+1] ;
if br(end) < M, br(end+1) = M ; end
for bi = 1:length(br)-1
  sel = br(bi) : br(bi+1)-1 ;

  %sqx  = sqrt(x(:, sel)) ;
  sqx  = x(:, sel).^(g/2) ;
  lgx  = log(x(:, sel) + eps) ;

  psi(1:d, sel) = skp0 * sqx ;
  for i=1:n
    llgx = l(i) * lgx ;
    psi(2*d*(i-1) + d   + (1:d), sel) = s2kp(i) * cos(llgx) .* sqx ;
    psi(2*d*(i-1) + 2*d + (1:d), sel) = s2kp(i) * sin(llgx) .* sqx ;
  end
end

% sanity check
if 0
  for j = 1:M
    for i = 1:d
      psi_((i-1)*sz + (1:sz), j) = ...
        sqrt(x(i,j)) * [ ...
        skp0, ...
        s2kp .* cos(l * log(x(i,j))), ...
        s2kp .* sin(l * log(x(i,j)))  ]' ;
    end
  end
  %keyboard
  psi = psi_ ;
end

