classdef vl_test_svmtrain < matlab.unittest.TestCase
  properties
    x
    y
    w
    b
    obj
    lambda
    biasMultiplier
  end
  
  methods (TestClassSetup)
    function setup(t)
      randn('state',0) ;
      Np = 10 ;
      Nn = 10 ;
      xp = diag([1 3])*randn(2, Np) ;
      xn = diag([1 3])*randn(2, Nn) ;
      xp(1,:) = xp(1,:) + 2 + 1 ;
      xn(1,:) = xn(1,:) - 2 + 1 ;
      
      t.x = [xp xn] ;
      t.y = [ones(1,Np) -ones(1,Nn)] ;
      t.lambda = 0.01 ;
      t.biasMultiplier = 10 ;
      
      if 0
        figure(1) ; clf;
        vl_plotframe(xp, 'g') ; hold on ;
        vl_plotframe(xn, 'r') ;
        axis equal ; grid on ;
      end
      
      % Run LibSVM as an accuate solver to compare results with.  Note that
      % LibSVM optimizes a slightly different cost function due to the way
      % the bias is handled.
      % [s.w, s.b] = accurate_solver(s.x, s.y, s.lambda, s.biasMultiplier) ;
      t.w = [1.180762951236242; 0.098366470721632] ;
      t.b = -1.540018443946204 ;
      t.obj = t.objf(t, t.w, t.b) ;
    end
  end
  
  methods (Test)
    
    function test_sgd_basic(t)
      for conv = {@single, @double}
        conv = conv{1} ;
        vl_twister('state',0) ;
        [w b info] = vl_svmtrain(t.x, t.y, t.lambda, ...
          'Solver', 'sgd', ...
          'BiasMultiplier', t.biasMultiplier, ...
          'BiasLearningRate', 1/t.biasMultiplier, ...
          'MaxNumIterations', 1e5, ...
          'Epsilon', 1e-3) ;
        % there are no absolute guarantees on the objective gap, but
        % the heuristic SGD uses as stopping criterion seems reasonable
        % within a factor 10 at least.
        o = t.objf(t, w, b) ;
        gap = o - t.obj ;
        t.verifyEqual(conv([w; b]), conv([t.w; t.b]), 'AbsTol', conv(0.1)) ;
        t.verifyTrue(gap <= 1e-2) ;
      end
    end
    
    function test_sdca_basic(t)
      for conv = {@single, @double}
        conv = conv{1} ;
        vl_twister('state',0) ;
        [w b info] = vl_svmtrain(t.x, t.y, t.lambda, ...
          'Solver', 'sdca', ...
          'BiasMultiplier', t.biasMultiplier, ...
          'MaxNumIterations', 1e5, ...
          'Epsilon', 1e-3) ;
        
        % the gap with the accurate solver cannot be
        % greater than the duality gap.
        o = t.objf(t, w, b) ;
        gap = o - t.obj ;
        t.verifyEqual(conv([w; b]), conv([t.w; t.b]), 'AbsTol', conv(0.1)) ;
        t.verifyTrue(gap <= 1e-3) ;
      end
    end
    
    function test_weights(t)
      for algo = {'sgd', 'sdca'}
        for conv = {@single, @double}
          conv = conv{1} ;
          vl_twister('state',0) ;
          numRepeats = 10 ;
          pos = find(t.y > 0) ;
          neg = find(t.y < 0) ;
          weights = ones(1, numel(t.y)) ;
          weights(pos) = numRepeats ;
          
          % simulate weighting by repeating positives
          [w b info] = vl_svmtrain(...
            t.x(:, [repmat(pos,1,numRepeats) neg]), ...
            t.y(:, [repmat(pos,1,numRepeats) neg]), ...
            t.lambda / (numel(pos) *numRepeats + numel(neg)) / (numel(pos) + numel(neg)), ...
            'Solver', 'sdca', ...
            'BiasMultiplier', t.biasMultiplier, ...
            'MaxNumIterations', 1e6, ...
            'Epsilon', 1e-4) ;
          
          % apply weigthing
          [w_ b_ info_] = vl_svmtrain(...
            t.x, ...
            t.y, ...
            t.lambda, ...
            'Solver', char(algo), ...
            'BiasMultiplier', t.biasMultiplier, ...
            'MaxNumIterations', 1e6, ...
            'Epsilon', 1e-4, ...
            'Weights', weights) ;
          t.verifyEqual(conv([w; b]), conv([w_; b_]), 'AbsTol', conv(0.05)) ;
        end
      end
    end
       
    function test_homkermap(t)
      for solver = {'sgd', 'sdca'}
        for conv = {@single,@double}
          conv = conv{1} ;
          dataset = vl_svmdataset(conv(t.x), 'homkermap', struct('order',1)) ;
          vl_twister('state',0) ;
          [w_ b_] = vl_svmtrain(dataset, t.y, t.lambda) ;
          
          x_hom = vl_homkermap(conv(t.x), 1) ;
          vl_twister('state',0) ;
          [w b] = vl_svmtrain(x_hom, t.y, t.lambda) ;
          t.verifyEqual([w; b],[w_; b_], 'AbsTol', 1e-7) ;
        end
      end
    end
  end
  
  methods (Static)
    function [w,b] = accurate_solver(X, y, lambda, biasMultiplier)
      addpath opt/libsvm/matlab/
      N = size(X,2) ;
      model = svmtrain(y', [(1:N)' X'*X], sprintf(' -c %f -t 4 -e 0.00001 ', 1/(lambda*N))) ;
      w = X(:,model.SVs) * model.sv_coef ;
      b = - model.rho ;
      format long ;
      disp('model w:')
      disp(w)
      disp('bias b:')
      disp(b)
    end
    
    function o = objf(s, w, b)
      o = (sum(w.*w) + b*b) * s.lambda / 2 +  mean(max(0, 1 - s.y .* (w'*s.x + b))) ;
    end
  end
end


