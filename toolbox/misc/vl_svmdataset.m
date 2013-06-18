function dataset = vl_svmdataset(x, varargin)
% VL_SVMDATASET  Construct advanced SVM dataset structure
%   DATASET = VL_SVMDATASET(X) wraps the matrix X into a dataset
%   structure. All the formats supported by VL_SVMTRAIN() are
%   suppported here as well.
%
%   DATASET = VL_SVMDATASET(X, 'homkermap', HOM) where HOM is a
%   structure with zero or more of the following fields: HOM.kernel,
%   HOM.order, HOM.window, HOM.gamma, HOM.period, See VL_HOMKERMAP()
%   for a discussion of these parameters. The effect is to use the
%   homogeneous kernel map to expand the data on the fly. This is
%   equivalent (albeit somewhat slower) to running VL_HOMKERMAP()
%   prior to learning. The main advantage is the significant memory
%   saving as the expanded data needs not to be stored in memory.
%
%   See:: VL_SVMTRAIN(), VL_HOMKERMAP(), VL_HELP().

% Author: Daniele Perrone and Andrea Vedaldi

opts.homkermap = [] ;
opts = vl_argparse(opts, varargin) ;

if issparse(x)
  error('X is a sparse matrix (sparse data will support be added in a future version.)') ;
end

if ~isa(x,'single') && ~isa(x,'double')
  error('X is neither SINGLE nor DOUBLE.') ;
end

sz = size(x) ;
if numel(sz) > 2
  error('X has more than two dimensions.') ;
end

dataset.data = x ;

if isstruct(opts.homkermap)
  if numel(opts.homkermap) ~= 1
    error('HOMKERMAP is not a singleton.') ;
  end
  ok = all(ismember(fieldnames(opts.homkermap), ...
                    {'kernel', 'order', 'window', 'gamma', 'period'})) ;
  if ~ok
    error('HOMKERMAP contains an unsupported field name.') ;
  end
  dataset.homkermap = opts.homkermap ;
elseif ~isempty(opts.homkermap)
    error('HOMKERMAP is not a structure nor []') ;
end
