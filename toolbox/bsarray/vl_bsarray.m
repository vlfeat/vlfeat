classdef vl_bsarray
% Class bsarray
%
% Copyright (C) 2012 Daniele Perrone
    % ---------------------------------------------------------------------
    properties(Constant)
    % ---------------------------------------------------------------------
        sparsityRatio = 2/3 ;
        FLOAT = uint32(1) ;
        INT32 = uint32(7) ;
        UINT32 = uint32(8) ;
    end
    %----------------------------------------------------------------------
    properties
    % ---------------------------------------------------------------------
        data = [];
    end

    % ---------------------------------------------------------------------
    methods
    % ---------------------------------------------------------------------
        % -----------------------------------------------------------------
	function self = vl_bsarray(transpose,varargin)
        % -----------------------------------------------------------------
            if (mod(length(varargin),3) ~= 0)
                error(['The input must be a sequence of pairs (block ' ...
                       'position, block data).']) ;
            end
            k = 1;
            input = cell(floor(length(varargin)*2/3),1) ;
            for i = 1:3:length(varargin)
                if (size(varargin{i+2},2) ~= 1)
                    error('Each block must be a one dimensional array.');
                end

                type = uint32(vl_bsarray.getBlockType(varargin{i+2})) ...
                       ;
                input{k} = [type; ...
                                   uint32(varargin{i+1}-1); ...
                                   uint32(varargin{i}); ...
                            uint32(length(varargin{i+2}));] ;



                switch type
                  case 1
                    input{k+1} = varargin{i+2} ;
                    k = k + 2;
                  case 2
                    input{k+1} = varargin{i+2}(1) ;
                    k = k + 2;
                  case 3
                    [r c v] = find(varargin{i+2});
                    iv = [r'; v'];

                    input{k} = [input{k}; uint32(size(iv,1))] ...
                        ;
                    input{k+1} = uint32(r) ;
                    input{k+2} = v ;

                    k = k + 3;
                end






            end


            self.data = vl_bsarray_new(uint32(transpose), ...
                                       uint32(floor(length(input)/2)), input) ...
                ;


        end


        % --------------------------------------------------------------
        function  res  = mtimes(a,b)
        % --------------------------------------------------------------
            if (isa(a,'vl_bsarray') && isa(b,'single'))
                res = vl_bsarray_mtimes(a.data,b) ;
            else
                error('Unsupported operation.') ;
            end

        end




        % --------------------------------------------------------------
        function  M  = full(self)
        % --------------------------------------------------------------

            M = vl_bsarray_full(self.data) ;

        end

        function self = ctranspose(self)

            self.data(1) = ~self.data(1);

        end



    end

    methods(Static)
        % --------------------------------------------------------------
        function sp = isSparse(A)
        % --------------------------------------------------------------
            sp = length(find(A)) <  vl_bsarray.sparsityRatio*length(A(:)) ;
        end

        % --------------------------------------------------------------
        function cs = isConstant(A)
        % --------------------------------------------------------------
            cs = (A == A(ones(size(A)))) ;
            cs = all(cs(:)) ;
        end

        % --------------------------------------------------------------
        function type = getBlockType(B)
        % --------------------------------------------------------------
            if ~vl_bsarray.isSparse(B)
                if ~vl_bsarray.isConstant(B)
                    type = 1 ;
                else
                    type = 2 ;
                end
            else
                type = 3 ;
            end
        end
    end

end
