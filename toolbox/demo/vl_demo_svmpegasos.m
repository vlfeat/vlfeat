% VL_DEMO_SVMPEGASOS  Demo: SVMPEGASOS: 2D linear learning
function vl_demo_svmpegasos

% Set up training data
Np = 200 ;
Nn = 200 ;
Xp = diag([1 3])*randn(2, Np) ;
Xn = diag([1 3])*randn(2, Nn) ;
Xp(1,:) = Xp(1,:) + 2  ;
Xn(1,:) = Xn(1,:) - 2  ;

X = [Xp Xn] ;
y = [ones(1,Np) -ones(1,Nn)] ;

figure(1)
plot(Xn(1,:),Xn(2,:),'*')
hold on
plot(Xp(1,:),Xp(2,:),'*r')
axis equal ; 
%axis off ; axis tight ;
vl_demo_print('pegasos_training') ;

% parameters
lambda = 0.01 ; 

% training
energy = [] ;
dataset = vl_maketrainingset(X, int8(y)) ;
[w b info] = vl_svmpegasos(dataset, lambda, ...
                           'MaxIterations',5000,...
                           'DiagnosticFunction',@diagnostics,...
                           'DiagnosticCallRef',energy) ;

figure(1) ;
x = min(X(1,:)):max(X(1,:)) ; 
hold on
set(line([0 w(1)], [0 w(2)]), 'color', 'y', 'linewidth', 4) ;
xlim([-3 3]) ;
ylim([-3 3]) ;
set(line(10*[w(2) -w(2)], 10*[-w(1) w(1)]), ...
    'color', 'y', 'linewidth', 2, 'linestyle', '-') ;
axis equal ;
hold off
%axis off ; axis tight ;
vl_demo_print('pegasos_res') ;

figure(2)
%axis equal ; 
%axis off ; axis tight ;
vl_demo_print('pegasos_energy') ;


function energy = diagnostics(svm,energy)
  figure(2) ; 
  %keyboard
  energy = [energy svm.energy] ;
  plot(energy) ;
  drawnow ;
