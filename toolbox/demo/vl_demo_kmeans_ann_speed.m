% VL_DEMO_KMEANS_ANN_SPEED   Compares Lloyd's, Elkan, and ANN k-means

numCenters = 200 ;
numTrials = 3 ;
maxNumIterations = 10 ;
initialization = 'randsel' ;
%initialization = 'plusplus';
distance = 'l2' ;

%% Create an example dataset

dimension = 32 ;
numData = 50000 ;

X = randn(dimension,numData);

%% Run various k-means algorithms on the data
algorithms = {'Lloyd','Elkan','ANN 1/4','ANN 1/10','ANN 1/50' } ;
options = {{'Algorithm', 'Lloyd'}, ...
           {'Algorithm', 'Elkan'}, ...
           {'Algorithm', 'ANN', 'MaxNumComparisons', ceil(numCenters / 4)}, ...
           {'Algorithm', 'ANN', 'MaxNumComparisons', ceil(numCenters / 10)}, ...
           {'Algorithm', 'ANN', 'MaxNumComparisons', ceil(numCenters / 50)}} ;
numCpus = [1 0] ;

clear time energy ;
for n = 1:2
  for a = 1:numel(algorithms)
    for t = 1:numTrials
      vl_threads(numCpus(n)) ;
      start = tic ;
      [C, A, E] = vl_kmeans(X, ...
                            numCenters, 'Verbose', ...
                            'Distance', distance, ...
                            'MaxNumIterations', maxNumIterations, ...
                            options{a}{:}) ;
      if vl_isoctave()
        time(t,a,n) = (tic() - start) / 1e6 ;
      else
        time(t,a,n) = toc(start) ;
      end
      energy(t,a,n) = E ;
    end
  end
end

% average over tirals
time = squeeze(mean(time,1)) ;
energy = squeeze(mean(energy,1)) ;

figure(1) ; clf ;
for n=1:2
  if n == 1
    str = 'Serial' ;
  else
    str = 'Parallel' ;
  end

  subplot(3,2,(n-1)+1) ;
  bar(time(:,n)) ;
  set(gca,'XTickLabel',algorithms);
  set(gca,'FontSize',8),
  xlabel('Algorithm');
  ylabel('Time [s]');
  title(str) ;

  subplot(3,2,(n-1)+3) ;
  bar(energy(:,n));
  set(gca,'XTickLabel',algorithms);
  set(gca,'FontSize',8),
  xlabel('Algorithm');
  ylabel('Energy');
  title(str) ;

  subplot(3,2,(n-1)+5) ;
  bar(time(1,1)./time(:,n)) ;
  set(gca,'XTickLabel',algorithms);
  set(gca,'FontSize',8),
  xlabel('Algorithm');
  ylabel('Speedup');
  title(str) ;
end

vl_demo_print('kmeans_speed',1);
