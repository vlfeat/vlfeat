classdef vl_test_sift < matlab.unittest.TestCase
  properties
    I
    ubc
  end
  
  methods (TestClassSetup)
    function ssetup(t)
      t.I = im2single(imread(fullfile(vl_root,'data','box.pgm'))) ;
      [t.ubc.f, t.ubc.d] = ...
        vl_ubcread(fullfile(vl_root,'data','box.sift')) ;
    end
  end
  
  methods (Test)
    function test_ubc_descriptor(t)
      err = [] ;
      [f, d] = vl_sift(t.I,...
        'firstoctave', -1, ...
        'frames', t.ubc.f) ;
      D2 = vl_alldist(f, t.ubc.f) ;
      [drop, perm] = min(D2) ;
      f = f(:,perm) ;
      d = d(:,perm) ;
      error = mean(sqrt(sum((single(t.ubc.d) - single(d)).^2))) ...
        / mean(sqrt(sum(single(t.ubc.d).^2))) ;
      t.verifyTrue(error < 0.1, ...
        'The SIFT descriptor did not produce desctiptors similar to UBC ones') ;
    end
    
    function test_ubc_detector(t)
      [f, d] = vl_sift(t.I,...
        'firstoctave', -1, ...
        'peakthresh', .01, ...
        'edgethresh', 10) ;
      
      t.ubc.f(4,:) = mod(t.ubc.f(4,:), 2*pi) ;
      f(4,:) = mod(f(4,:), 2*pi) ;
      
      % scale the components so that 1 pixel erro in x,y,z is equal to a
      % 10-th of angle.
      S = diag([1 1 1 20/pi]);
      D2 = vl_alldist(S * t.ubc.f, S * f) ;
      [d2,perm] = sort(min(D2)) ;
      error = sqrt(d2) ;
      quant80 = round(.8 * size(f,2)) ;
      
      % check for less than one pixel error at 80% quantile
      t.verifyTrue(error(quant80) < 1, ...
        'The SIFT detector did not produce enough keypoints similar to UBC ones') ;
    end
  end
end


