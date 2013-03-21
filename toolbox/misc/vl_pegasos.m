% VL_PEGASOS [deprecated]
% VL_PEGASOS is deprecated. Please use VL_SVMTRAIN() instead.

function [w b info] = vl_pegasos(X,Y,LAMBDA, varargin)

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

% parameters for vl_maketrainingset
setvarargin = {};
if (sum(strcmpi('HOMKERMAP',varargin)))
    setvarargin{end+1} = 'HOMKERMAP';
    setvarargin{end+1} = varargin{find(strcmpi('HOMKERMAP',varargin),1)+1};
    varargin(find(strcmpi('HOMKERMAP',varargin),1)+1)=[];
    varargin(find(strcmpi('HOMKERMAP',varargin),1))=[];
end
if (sum(strcmpi('KChi2',varargin)))
    setvarargin{end+1} = 'KChi2';
    varargin(find(strcmpi('KChi2',varargin),1))=[];
end
if (sum(strcmpi('KINTERS',varargin)))
    setvarargin{end+1} = 'KINTERS';
    varargin(find(strcmpi('KINTERS',varargin),1))=[];
end
if (sum(strcmpi('KL1',varargin)))
    setvarargin{end+1} = 'KL1';
    varargin(find(strcmpi('KL1',varargin),1))=[];
end
if (sum(strcmpi('KJS',varargin)))
    setvarargin{end+1} = 'KJS';
    varargin(find(strcmpi('KJS',varargin),1))=[];
end
if (sum(strcmpi('Period',varargin)))
    setvarargin{end+1} = 'Period';
    setvarargin{end+1} = varargin{find(strcmpi('Period',varargin),1)+1};
    varargin(find(strcmpi('Period',varargin),1)+1)=[];
    varargin(find(strcmpi('Period',varargin),1))=[];
end
if (sum(strcmpi('Window',varargin)))
    setvarargin{end+1} = 'Window';
    setvarargin{end+1} = varargin{find(strcmpi('Window',varargin),1)+1};
    varargin(find(strcmpi('Window',varargin),1)+1)=[];
    varargin(find(strcmpi('Window',varargin),1))=[];
end
if (sum(strcmpi('Gamma',varargin)))
    setvarargin{end+1} = 'Gamma';
    setvarargin{end+1} = varargin{find(strcmpi('Gamma',varargin),1)+1};
    varargin(find(strcmpi('Gamma',varargin),1)+1)=[];
    varargin(find(strcmpi('Gamma',varargin),1))=[];
end

setvarargin{:}

DATA = vl_maketrainingset(double(X),int8(Y),setvarargin{:});
DATA
[w b info] = vl_svmtrain(DATA,LAMBDA,varargin{:});


fprintf('\n vl_pegasos is DEPRECATED. Please use vl_svmtrain instead. \n\n');

end
