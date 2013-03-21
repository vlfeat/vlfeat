% VL_SVMPEGASOS [deprecated]
% VL_SVMPEGASOS is deprecated. Please use VL_SVMTRAIN() instead.



function [w b info] = vl_svmpegasos(DATA,LAMBDA, varargin)

% Verbose not supported
if (sum(strcmpi('Verbose',varargin)))
    varargin(find(strcmpi('Verbose',varargin),1))=[];
    fprintf('Option VERBOSE is no longer supported.\n');
end

% DiagnosticCallRef not supported
if (sum(strcmpi('DiagnosticCallRef',varargin)))
    varargin(find(strcmpi('DiagnosticCallRef',varargin),1)+1)=[];
    varargin(find(strcmpi('DiagnosticCallRef',varargin),1))=[];
    fprintf('Option DIAGNOSTICCALLREF is no longer supported.\n Please follow the VLFeat tutorial on SVMs for more information on diagnostics\n');
end

% different default value for MaxIterations
if (sum(strcmpi('MaxIterations',varargin)) == 0)
    varargin{end+1} = 'MaxIterations';
    varargin{end+1} = ceil(10/LAMBDA);
end

% different default value for BiasMultiplier
if (sum(strcmpi('BiasMultiplier',varargin)) == 0)
    varargin{end+1} = 'BiasMultiplier';
    varargin{end+1} = 0;
end

[w b info] = vl_svmtrain(DATA,LAMBDA,varargin{:});

fprintf('\n vl_svmpegasos is DEPRECATED. Please use vl_svmtrain instead. \n\n');

end
