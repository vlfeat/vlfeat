function results = vl_test_svmtrain(varargin)
    % VL_TEST_SVMTRAIN
    vl_test_init ;
end

function s = setup()
    randn('state',0) ;

    s.biasMultiplier = 10 ;
    s.lambda = 0.01 ;

    Np = 10 ;
    Nn = 10 ;
    Xp = diag([1 3])*randn(2, Np) ;
    Xn = diag([1 3])*randn(2, Nn) ;
    Xp(1,:) = Xp(1,:) + 2 + 1 ;
    Xn(1,:) = Xn(1,:) - 2 + 1 ;

    s.X = [Xp Xn] ;
    s.y = [ones(1,Np) -ones(1,Nn)] ;
    %s.w = exact_solver(s.X, s.y, s.lambda, s.biasMultiplier)
    s.w = [1.181106685845652 ;
           0.098478251033487 ;
           -0.154057992404545 ] ;
end   

% % % SGD % % % 

function test_sgd_problem_1(s)
    for conv = {@single,@double}
      vl_twister('state',0) ;
      conv = conv{1} ;
      dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
      [w b info] = vl_svmtrain(dataset, s.lambda, ...
                     'MaxIterations', 100000, ...
                     'BiasMultiplier', s.biasMultiplier, ...
                     'BiasPreconditioner', .1) ;

      % test input
      vl_assert_equal(info.biasMultiplier,s.biasMultiplier); 
      vl_assert_almost_equal(info.biasPreconditioner,.1,1e-3); 
      vl_assert_almost_equal(conv([w; b]), conv(s.w), 0.1) ;
    end
end

function test_sgd_continue_training(s)
    for conv = {@single,@double}
      conv = conv{1} ;

      vl_twister('state',0) ;
      dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
      [w b] = vl_svmtrain(dataset, s.lambda, ...
                     'MaxIterations', 3000, ...
                     'BiasMultiplier', s.biasMultiplier) ;

      vl_twister('state',0) ;
      [w1 b1] = vl_svmtrain(dataset, s.lambda, ...
                     'StartingIteration', 1, ...
                     'MaxIterations', 1500, ...
                      'BiasMultiplier', s.biasMultiplier) ;
      [w2 b2] = vl_svmtrain(dataset, s.lambda, ...
                      'StartingIteration', 1501, ...
                      'StartingModel', w1, ...
                      'StartingBias', b1, ...
                      'MaxIterations', 3000, ...
                      'BiasMultiplier', s.biasMultiplier) ;
      vl_assert_almost_equal([w; b],[w2; b2],1e-7) ;
    end
end

function test_sgd_continue_training_with_perm(s)
    perm = uint32(randperm(size(s.X,2))) ;
    for conv = {@single,@double}
      conv = conv{1} ;

      vl_twister('state',0) ;
      dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
      [w b] = vl_svmtrain(dataset, s.lambda, ...
                     'MaxIterations', 3000, ...
                     'BiasMultiplier', s.biasMultiplier, ...
                     'Permutation', perm) ;

      vl_twister('state',0) ;
      [w1 b1] = vl_svmtrain(dataset, s.lambda, ...
                     'StartingIteration', 1, ...
                     'MaxIterations', 1500, ...
                     'BiasMultiplier', s.biasMultiplier, ...
                      'Permutation', perm) ;
      [w2 b2] = vl_svmtrain(dataset, s.lambda, ...
                      'StartingIteration', 1501, ...
                      'StartingModel', w1, ...
                      'StartingBias', b1, ...
                      'MaxIterations', 3000, ...
                      'BiasMultiplier', s.biasMultiplier, ...
                      'Permutation', perm) ;

      vl_assert_almost_equal([w; b],[w2; b2],1e-7) ;
    end
end

function test_sgd_homkermap(s)
  for conv = {@single,@double}
    vl_twister('state',0) ;
    conv = conv{1} ;
    dataset = vl_maketrainingset(conv(s.X), int8(s.y),...
                                 'homkermap',1,...
                                 'gamma',.5,...
                                 'kchi2') ;
    [we be] = vl_svmtrain(dataset, s.lambda, ...
                   'MaxIterations', 100000, ...
                   'BiasMultiplier', s.biasMultiplier, ...
                   'BiasPreconditioner', .1) ;
    vl_twister('state',0) ;
    X_homker = vl_homkermap(conv(s.X), 1, 'kchi2', 'gamma', .5) ;
    dataset_homker = vl_maketrainingset(conv(X_homker), int8(s.y));
    [w b] = vl_svmtrain(dataset_homker, s.lambda, ...
                   'MaxIterations', 100000, ...
                   'BiasMultiplier', s.biasMultiplier, ...
                   'BiasPreconditioner', 0.1);
 
    vl_assert_almost_equal([w; b],[we; be], 1e-7) ;
  end
end

function test_sgd_epsilon(s)
    for conv = {@single,@double}
      vl_twister('state',0) ;
      conv = conv{1} ;

      dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
      [w b info] = vl_svmtrain(dataset, s.lambda, ...
                            'MaxIterations', 1000000, ...
                            'BiasMultiplier', s.biasMultiplier, ...
                            'BiasPreconditioner', .1) ;
      vl_twister('state',0) ;
      [we be infoe] = vl_svmtrain(dataset, s.lambda, ...
                              'MaxIterations', 1000000, ...
                              'Epsilon',1e-7,...
                              'BiasMultiplier', s.biasMultiplier, ...
                              'BiasPreconditioner', .1) ;
      vl_assert_almost_equal([w; b], [we; be], 1e-2) ;
      assert(info.iterations > infoe.iterations);
    end
end



% % % SDCA % % %

function test_sdca_problem_1(s)
    for conv = {@single,@double}
      vl_twister('state',0) ;
      conv = conv{1} ;
      dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
      [w b info] = vl_svmtrain(dataset, s.lambda,  'DCA',...
                     'MaxIterations', 100000, ...
                     'BiasMultiplier', s.biasMultiplier) ;

      % test input
      vl_assert_equal(info.biasMultiplier,s.biasMultiplier); 
      vl_assert_almost_equal(conv([w; b]), conv(s.w), 0.1) ;
    end
end


function test_sdca_homkermap(s)
  for conv = {@single,@double}
    vl_twister('state',0) ;
    conv = conv{1} ;
    dataset = vl_maketrainingset(conv(s.X), int8(s.y),...
                                 'homkermap',1,...
                                 'gamma',.5,...
                                 'kchi2') ;
    [we be] = vl_svmtrain(dataset, s.lambda, 'DCA',...
                   'MaxIterations', 100000, ...
                   'BiasMultiplier', s.biasMultiplier) ;
    vl_twister('state',0) ;
    X_homker = vl_homkermap(conv(s.X), 1, 'kchi2', 'gamma', .5) ;
    dataset_homker = vl_maketrainingset(conv(X_homker), int8(s.y));
    [w b] = vl_svmtrain(dataset_homker, s.lambda, 'DCA',...
                   'MaxIterations', 100000, ...
                   'BiasMultiplier', s.biasMultiplier);
 
    vl_assert_almost_equal([w; b],[we; be], 1e-7) ;
  end
end

function test_sdca_epsilon(s)
    for conv = {@single,@double}
      vl_twister('state',0) ;
      conv = conv{1} ;

      dataset = vl_maketrainingset(conv(s.X), int8(s.y)) ;
      [w b info] = vl_svmtrain(dataset, s.lambda, 'DCA', ...
                            'MaxIterations', 1000000, ...
                            'BiasMultiplier', s.biasMultiplier) ;
      vl_twister('state',0) ;
      [we be infoe] = vl_svmtrain(dataset, s.lambda, 'DCA', ...
                              'MaxIterations', 1000000, ...
                              'Epsilon',1e-7,...
                              'BiasMultiplier', s.biasMultiplier) ;
      vl_assert_almost_equal([w; b], [we; be], 1e-2) ;
      assert(info.iterations > infoe.iterations);
    end
end



function w = exact_solver(X, y, lambda, biasMultiplier)
    N = size(X,2) ;
    model = svmtrain(y', [(1:N)' X'*X], sprintf(' -c %f -t 4 ', 1/(lambda*N))) ;
    w = X(:,model.SVs) * model.sv_coef ;
    w(3) = - model.rho / biasMultiplier ;
    format long ;
    disp('model w:')
    disp(w)
end
