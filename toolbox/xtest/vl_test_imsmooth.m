function results = vl_test_imsmooth(varargin)
% VL_TEST_IMSMOOTH
vl_test_init ;

function s = setup()
I = im2double(imread(fullfile(vl_root,'data','spots.jpg'))) ;
I = max(min(vl_imdown(I),1),0) ;
s.I = single(I) ;

function test_pad_by_continuity(s)
% Convolving a constant signal padded with continuity does not change
% the signal.
I = ones(3) ;
for ker = {'triangular', 'gaussian'}
  ker = char(ker) ;
  J  = vl_imsmooth(I, 2, ...
                   'kernel', ker, ...
                   'padding', 'continuity') ;
  vl_assert_almost_equal(J, I, 1e-4, ...
                         'padding by continutiy with kernel = %s', ker) ;
end

function test_kernels(s)
for ker = {'triangular', 'gaussian'}
  ker = char(ker) ;
  for type = {@single, @double}
    for simd = [0 1]
      for sigma = [1 2 7]
        for step = [1 2 3]
          vl_simdctrl(simd) ;
          conv = type{1} ;
          g = equivalent_kernel(ker, sigma) ;
          J  = vl_imsmooth(conv(s.I), sigma, ...
                           'kernel', ker, ...
                           'padding', 'zero', ...
                           'subsample', step) ;
          J_ = conv(convolve(s.I, g, step)) ;
          vl_assert_almost_equal(J, J_, 1e-4, ...
                                 'kernel=%s sigma=%f step=%d simd=%d', ...
                                 ker, sigma, step, simd) ;
        end
      end
    end
  end
end

function g = equivalent_kernel(ker, sigma)
switch ker
  case 'gaussian'
    W = ceil(4*sigma) ;
    g = exp(-.5*((-W:W)/(sigma+eps)).^2) ;
  case 'triangular'
    W = max(round(sigma),1) ;
    g = W - abs(-W+1:W-1) ;
end
g = g / sum(g) ;

function I = convolve(I, g, step)
if strcmp(class(I),'single')
  g = single(g) ;
else
  g = double(g) ;
end
for k=1:size(I,3)
  I(:,:,k) = conv2(g,g,I(:,:,k),'same');
end
I = I(1:step:end,1:step:end,:) ;
