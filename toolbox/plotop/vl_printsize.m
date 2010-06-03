function vl_printsize(varargin)
% VL_PRINTSIZE  Set the print size of a figure
%   VL_PRINTSIZE(R) sets the PaperPosition property of the current
%   figure so that the width of the figure is the fraction R of a
%   'uslsetter' page. It also sets the PaperSize property to tightly
%   match the figure size. In this way, printing to any format crops
%   the figure as printing to EPS would do.
%
%   VL_PRINTSIZE(FIG,R) opearates on the specified figure FIG.
%
%   This command is useful to resize a figure before printing it so
%   that elements are scaled appropriately to match the desired figure
%   size in print. The function accepts the following optional
%   arguments:
%
%   Aspect:: none
%     Change the figure aspect ratio (widht/height) to the specified
%     value.
%
%   Reference:: vertical
%     If set to 'horizontal' the ratio R is the widht of the figure
%     over the width of the page. If 'vertical', the ratio R is the
%     height of the figure over the height of the page.
%
%   PaperType:: letter
%     Set the type (size) of the reference paperReference. Any of the
%     paper types supported by MATLAB can be used (see PRINT())).
%
%   See also:: VL_HELP().

% AUTORIGHTS
% Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson
%
% This file is part of VLFeat, available under the terms of the
% GNU GPLv2, or (at your option) any later version.

if length(varargin) >= 2 && isnumeric(varargin{2})
  % called with two numeric arguments
  fig = varargin{1} ;
  varargin(1) = [] ;
else
  fig = gcf ;
end

sizeRatio = varargin{1} ;
varargin(1) = [] ;

aspectRatio = NaN ;
reference = 'horizontal' ;
paperType = 'usletter' ;

for ai=1:2:length(varargin)
  opt = lower(varargin{ai}) ;
  arg = varargin{ai+1} ;
  switch lower(opt)
    case 'aspect'
      aspectRatio = opt ;
    case 'reference'
      switch lower(arg)
        case 'horizontal'
          reference = 'horizontal' ;
        case 'vertical'
          reference = 'vertical' ;
        otherwise
          error('Invalid value ''%s'' for option ''%s''.', arg, opt) ;
      end
    case 'papertype'
      paperType = opt ;
    otherwise
      error('Unknown option ''%s''.', opt) ;
  end
end


% set the paper size to the reference type
set(fig, 'PaperType', paperType) ;
paperSize = get(fig, 'PaperSize') ;

% get the current figure position to compute the current aspect ratio
position = get(fig, 'PaperPosition') ;

% if not specified, compute current aspect ratio
if isnan(aspectRatio)
  aspectRatio = position(3) / position(4) ;
end

% place the figure in the bottom-left corner
position(1:2) = 0 ;

% resize the figure
switch reference
  case 'horizontal'
    s = paperSize(1) / position(3) * sizeRatio ;
  case 'vertical'
    s = paperSize(2) / position(4) * sizeRatio ;
end
position(3:4) = position(3) * s * [1 1/aspectRatio] ;

set(fig, 'PaperPosition', position, ...
         'PaperSize', position(3:4)) ;
end
