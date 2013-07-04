function results = vl_test_svmtrain(varargin)
% VL_TEST_SVMTRAIN
  vl_test_init ;
end

function s = setup()
  randn('state',0) ;
  Np = 10 ;
  Nn = 10 ;
  xp = diag([1 3])*randn(2, Np) ;
  xn = diag([1 3])*randn(2, Nn) ;
  xp(1,:) = xp(1,:) + 2 + 1 ;
  xn(1,:) = xn(1,:) - 2 + 1 ;

  s.x = [xp xn] ;
  s.y = [ones(1,Np) -ones(1,Nn)] ;
  s.lambda = 0.01 ;
  s.biasMultiplier = 10 ;

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
  s.w = [1.180762951236242; 0.098366470721632] ;
  s.b = -1.540018443946204 ;
  s.obj = obj(s, s.w, s.b) ;
end

function test_sgd_basic(s)
  for conv = {@single, @double}
    conv = conv{1} ;
    vl_twister('state',0) ;
    [w b info] = vl_svmtrain(s.x, s.y, s.lambda, ...
                             'Solver', 'sgd', ...
                             'BiasMultiplier', s.biasMultiplier, ...
                             'BiasLearningRate', 1/s.biasMultiplier, ...
                             'MaxNumIterations', 1e5, ...
                             'Epsilon', 1e-3) ;
    % there are no absolute guarantees on the objective gap, but
    % the heuristic SGD uses as stopping criterion seems reasonable
    % within a factor 10 at least.
    o = obj(s, w, b) ;
    gap = o - s.obj ;
    vl_assert_almost_equal(conv([w; b]), conv([s.w; s.b]), 0.1) ;
    assert(gap <= 1e-2) ;
  end
end

function test_sdca_basic(s)
  for conv = {@single, @double}
    conv = conv{1} ;
    vl_twister('state',0) ;
    [w b info] = vl_svmtrain(s.x, s.y, s.lambda, ...
                             'Solver', 'sdca', ...
                             'BiasMultiplier', s.biasMultiplier, ...
                             'MaxNumIterations', 1e5, ...
                             'Epsilon', 1e-3) ;

    % the gap with the accurate solver cannot be
    % greater than the duality gap.
    o = obj(s, w, b) ;
    gap = o - s.obj ;
    vl_assert_almost_equal(conv([w; b]), conv([s.w; s.b]), 0.1) ;
    assert(gap <= 1e-3) ;
  end
end

function test_weights(s)
  for algo = {'sgd', 'sdca'}
    for conv = {@single, @double}
      conv = conv{1} ;
      vl_twister('state',0) ;
      numRepeats = 10 ;
      pos = find(s.y > 0) ;
      neg = find(s.y < 0) ;
      weights = ones(1, numel(s.y)) ;
      weights(pos) = numRepeats ;

      % simulate weighting by repeating positives
      [w b info] = vl_svmtrain(...
        s.x(:, [repmat(pos,1,numRepeats) neg]), ...
        s.y(:, [repmat(pos,1,numRepeats) neg]), ...
        s.lambda / (numel(pos) *numRepeats + numel(neg)) / (numel(pos) + numel(neg)), ...
        'Solver', 'sdca', ...
        'BiasMultiplier', s.biasMultiplier, ...
        'MaxNumIterations', 1e6, ...
        'Epsilon', 1e-4) ;

      % apply weigthing
      [w_ b_ info_] = vl_svmtrain(...
        s.x, ...
        s.y, ...
        s.lambda, ...
        'Solver', char(algo), ...
        'BiasMultiplier', s.biasMultiplier, ...
        'MaxNumIterations', 1e6, ...
        'Epsilon', 1e-4, ...
        'Weights', weights) ;
      vl_assert_almost_equal(conv([w; b]), conv([w_; b_]), 0.05) ;
    end
  end
end

% function test_sgd_continue_training(s)
%     for conv = {@single,@double}
%       conv = conv{1} ;

%       vl_twister('state',0) ;
%       dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
%       [w b] = vl_svmtrain(dataset, s.lambda, ...
%                      'MaxIterations', 3000, ...
%                      'BiasMultiplier', s.biasMultiplier) ;

%       vl_twister('state',0) ;
%       [w1 b1] = vl_svmtrain(dataset, s.lambda, ...
%                      'StartingIteration', 1, ...
%                      'MaxIterations', 1500, ...
%                       'BiasMultiplier', s.biasMultiplier) ;
%       [w2 b2] = vl_svmtrain(dataset, s.lambda, ...
%                       'StartingIteration', 1501, ...
%                       'StartingModel', w1, ...
%                       'StartingBias', b1, ...
%                       'MaxIterations', 3000, ...
%                       'BiasMultiplier', s.biasMultiplier) ;
%       vl_assert_almost_equal([w; b],[w2; b2],1e-7) ;
%     end
% end

% function test_sgd_continue_training_with_perm(s)
%     perm = uint32(randperm(size(s.X,2))) ;
%     for conv = {@single,@double}
%       conv = conv{1} ;

%       vl_twister('state',0) ;
%       dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
%       [w b] = vl_svmtrain(dataset, s.lambda, ...
%                      'MaxIterations', 3000, ...
%                      'BiasMultiplier', s.biasMultiplier, ...
%                      'Permutation', perm) ;

%       vl_twister('state',0) ;
%       [w1 b1] = vl_svmtrain(dataset, s.lambda, ...
%                      'StartingIteration', 1, ...
%                      'MaxIterations', 1500, ...
%                      'BiasMultiplier', s.biasMultiplier, ...
%                       'Permutation', perm) ;
%       [w2 b2] = vl_svmtrain(dataset, s.lambda, ...
%                       'StartingIteration', 1501, ...
%                       'StartingModel', w1, ...
%                       'StartingBias', b1, ...
%                       'MaxIterations', 3000, ...
%                       'BiasMultiplier', s.biasMultiplier, ...
%                       'Permutation', perm) ;

%       vl_assert_almost_equal([w; b],[w2; b2],1e-7) ;
%     end
% end

function test_homkermap(s)
  for solver = {'sgd', 'sdca'}
    for conv = {@single,@double}
      conv = conv{1} ;
      dataset = vl_svmdataset(conv(s.x), 'homkermap', struct('order',1)) ;
      vl_twister('state',0) ;
      [w_ b_] = vl_svmtrain(dataset, s.y, s.lambda) ;

      x_hom = vl_homkermap(conv(s.x), 1) ;
      vl_twister('state',0) ;
      [w b] = vl_svmtrain(x_hom, s.y, s.lambda) ;
      vl_assert_almost_equal([w; b],[w_; b_], 1e-7) ;
    end
  end
end

% function test_sgd_epsilon(s)
%     for conv = {@single,@double}
%       vl_twister('state',0) ;
%       conv = conv{1} ;

%       dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
%       [w b info] = vl_svmtrain(dataset, s.lambda, ...
%                             'MaxIterations', 1000000, ...
%                             'BiasMultiplier', s.biasMultiplier, ...
%                             'BiasPreconditioner', .1) ;
%       vl_twister('state',0) ;
%       [we be infoe] = vl_svmtrain(dataset, s.lambda, ...
%                               'MaxIterations', 1000000, ...
%                               'Epsilon',1e-7,...
%                               'BiasMultiplier', s.biasMultiplier, ...
%                               'BiasPreconditioner', .1) ;
%       vl_assert_almost_equal([w; b], [we; be], 1e-2) ;
%       assert(info.iterations > infoe.iterations);
%     end
% end



% % % % SDCA % % %

% function test_sdca_problem_1(s)
%     for conv = {@single,@double}
%       vl_twister('state',0) ;
%       conv = conv{1} ;
%       dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
%       [w b info] = vl_svmtrain(dataset, s.lambda,  'DCA',...
%                      'MaxIterations', 100000, ...
%                      'BiasMultiplier', s.biasMultiplier) ;

%       % test input
%       vl_assert_equal(info.biasMultiplier,s.biasMultiplier);
%       vl_assert_almost_equal(conv([w; b]), conv(s.w), 0.1) ;
%     end
% end


% function test_sdca_homkermap(s)
%   for conv = {@single,@double}
%     vl_twister('state',0) ;
%     conv = conv{1} ;
%     dataset = vl_maketrainingset(conv(s.X), int8(s.y),...
%                                  'homkermap',1,...
%                                  'gamma',.5,...
%                                  'kchi2') ;
%     [we be] = vl_svmtrain(dataset, s.lambda, 'DCA',...
%                    'MaxIterations', 100000, ...
%                    'BiasMultiplier', s.biasMultiplier) ;
%     vl_twister('state',0) ;
%     X_homker = vl_homkermap(conv(s.X), 1, 'kchi2', 'gamma', .5) ;
%     dataset_homker = vl_maketrainingset(conv(X_homker), int8(s.y));
%     [w b] = vl_svmtrain(dataset_homker, s.lambda, 'DCA',...
%                    'MaxIterations', 100000, ...
%                    'BiasMultiplier', s.biasMultiplier);

%     vl_assert_almost_equal([w; b],[we; be], 1e-7) ;
%   end
% end

% function test_sdca_epsilon(s)
%     for conv = {@single,@double}
%       vl_twister('state',0) ;
%       conv = conv{1} ;

%       dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
%       [w b info] = vl_svmtrain(dataset, s.lambda, 'DCA', ...
%                             'MaxIterations', 1000000, ...
%                             'BiasMultiplier', s.biasMultiplier) ;
%       vl_twister('state',0) ;
%       [we be infoe] = vl_svmtrain(dataset, s.lambda, 'DCA', ...
%                               'MaxIterations', 1000000, ...
%                               'Epsilon',1e-7,...
%                               'BiasMultiplier', s.biasMultiplier) ;
%       vl_assert_almost_equal([w; b], [we; be], 1e-2) ;
%       assert(info.iterations > infoe.iterations);
%     end
% end


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

function o = obj(s, w, b)
  o = (sum(w.*w) + b*b) * s.lambda / 2 +  mean(max(0, 1 - s.y .* (w'*s.x + b))) ;
end
