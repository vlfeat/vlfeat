classdef vl_test_imsmooth < matlab.unittest.TestCase
  properties
    I
  end
  
  methods (TestClassSetup)
    function setup(t)
      I = im2double(imread(fullfile(vl_root,'data','spots.jpg'))) ;
      I = max(min(vl_imdown(I),1),0) ;
      t.I = single(I) ;
    end
  end
  
  methods (Test)
    function test_pad_by_continuity(t)
      % Convolving a constant signal padded with continuity does not change
      % the signal.
      I = ones(3) ;
      for ker = {'triangular', 'gaussian'}
        ker = char(ker) ;
        J  = vl_imsmooth(I, 2, ...
          'kernel', ker, ...
          'padding', 'continuity') ;
        t.verifyEqual(J, I, 'AbsTol', cast(1e-4,'like',J), ...
          sprintf('padding by continutiy with kernel = %s', ker)) ;
      end
    end
    
    function test_kernels(t)
      for ker = {'triangular', 'gaussian'}
        ker = char(ker) ;
        for type = {@single, @double}
          for simd = [0 1]
            for sigma = [1 2 7]
              for step = [1 2 3]
                vl_simdctrl(simd) ;
                conv = type{1} ;
                g = t.equivalent_kernel(ker, sigma) ;
                J  = vl_imsmooth(conv(t.I), sigma, ...
                  'kernel', ker, ...
                  'padding', 'zero', ...
                  'subsample', step) ;
                J_ = conv(t.convolve(t.I, g, step)) ;
                t.verifyEqual(J, J_, 'AbsTol', cast(1e-4,'like',J), ...
                  sprintf('kernel=%s sigma=%f step=%d simd=%d', ...
                  ker, sigma, step, simd)) ;
              end
            end
          end
        end
      end
    end
  end

  methods (Static)
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
    end
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
    end
  end
end
