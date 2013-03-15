% VL_DEMO_SVM  Demo: SVM: 2D linear learning
function vl_demo_svm
y=[];X=[];

% Load training data X and their labels y
load('vl_demo_svm_data.mat')


Xp = X(:,y==1);
Xn = X(:,y==-1);

figure
plot(Xn(1,:),Xn(2,:),'*r')
hold on
plot(Xp(1,:),Xp(2,:),'*b')
axis equal ;
vl_demo_print('svm_training') ;
% Parameters
lambda = 0.01 ; % Regularization parameter
maxIter = 30 ; % Maximum number of iterations


% Diagnostic function
function diagnostics(svm)
  energy = [energy [svm.energy ; svm.energyDual ; svm.dualityGap ] ] ;
end

% Training the SVM
energy = [] ;
dataset = vl_maketrainingset(X, int8(y)) ;
[w b info] = vl_svmtrain(dataset, lambda, 'DCA',...
                           'MaxIterations',maxIter,...
                           'DiagnosticFunction',@diagnostics,...
                           'EnergyFreq',1)

% Visualisation
eq = [num2str(w(1)) '*x+' num2str(w(2)) '*y+' num2str(b)];
    
line = ezplot(eq, [-0.9 0.9 -0.9 0.9]);
set(line, 'Color', [0 0.8 0],'linewidth', 2);

vl_demo_print('svm_training_result') ;


figure 
hold on
plot(energy(1,:),'--b') ;
plot(energy(2,:),'-.g') ;
plot(energy(3,:),'r') ;
legend('Primal energy','Dual energy','Duality gap')
xlabel('Diagnostics iteration')
ylabel('Energy')
vl_demo_print('svm_energy') ;

end