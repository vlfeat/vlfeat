% VL_MAKETRAININGSET Make Training Dataset
%   D = VL_MAKETRAININGSET(X, Y) creates a training set struct D, given
%   the training vectors X and their labels Y. The training set D has
%   the fields DATA and LABELS. 
% 
%   If the HOMKERMAP option is used, a third field MAP is present in
%   D. MAP is another struct with the following fields: 
% 
%   order::
%     Order of the homogeneous kernel map.
% 
%   kernelType::
%     Type of kernel.
% 
%   windowType::
%     Type of window.
% 
%   gamma::
%     Value of the parameter GAMMA.
% 
%   period::
%     Value of the parameter PERIOD.
% 
%   VL_MAKETRAININGSET() accepts the following options:
% 
%   HOMKERMAP:: [empty]
%     Specify the use of an Homogeneus Kernel map for the training
%     data (See [2],[3]). The passed value N is such that a 2*N+1
%     dimensional approximated kernel map is computed. Each
%     training data point is expanded online into a vector of
%     dimension 2*N+1.
%  
%   KChi2::
%     Compute the map for the Chi2 kernel.
%
%   KINTERS::
%     Compute the map for the intersection kernel.
%
%   KL1::
%     Same as KINTERS, but deprecated as the name is not fully
%     accurate.
%
%   KJS::
%     Compute the map for the JS (Jensen-Shannon) kernel.
%
%   Period:: [automatically tuned]
%     Set the period of the kernel specturm. The approximation is
%     based on periodicizing the kernel specturm. If not specified,
%     the period is automatically set based on the heuristic described
%     in [2].
%
%   Window:: [RECTANGULAR]
%     Set the window used to truncate the spectrum before The window
%     can be either RECTANGULAR or UNIFORM window. See [2] and the API
%     documentation for details.
%
%   Gamma:: [1]
%     Set the homogeneity degree of the kernel. The standard kernels
%     are 1-homogeneous, but sometimes smaller values perform better
%     in applications. See [2] for details.
% 
%   Example::
%     VL_MAKETRAININGSET must be used together with VL_SVMPEGASOS,  
%
%       vl_twister('state',0) ;
%       dataset = vl_maketrainingset(x,y) ;
%       w = vl_svmpegasos(dataset,lambda,'NumIterations',1000) ;
% 
%   See also: VL_SVMPEGASOS(), VL_HOMKERMAP(), VL_HELP().

% AUTHORIGHTS