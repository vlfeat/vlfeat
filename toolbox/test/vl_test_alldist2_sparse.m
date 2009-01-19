function vl_test_alldist2_sparse
  
for entryStat = {'dense', 'sparse'}
  for norm = {'L2', 'L1', 'L0', 'chi2', 'kl2', 'kl1', 'kchi2'}
    for input = {'mixed', 'same'}
      norm = char(norm) ;
      entryStat = char(entryStat) ;
      input = char(input) ;
      
      args = {} ;
      
      X = rand(117, 43) ;
      Y = rand(117, 43) ;      

      if strcmp(entryStat, 'sparse')
        X(X < .5) = 0 ;
        Y(Y < .5) = 0 ;
      end      
      if strcmp(input, 'same')
        args  = {X, norm} ;
        args_ = {sparse(X), norm} ;
      else
        args  = {X, Y, norm} ;
        args_ = {sparse(X), sparse(Y), norm} ;
      end
      
      M  = vl_alldist2(args{:}) ;
      M_ = vl_alldist2(args_{:}) ;
      vl_test_assert(isequal(M, M_), ...
                     sprintf('vl_alldist entry_stats=%s norm=%s input=%s', ...
                             entryStat, norm, input)) ;
    end
  end
end
