function vl_printsize(varargin)
% VL_PRINTSIZE  Set the print size of a figure
%   VL_PRINTSIZE(R) sets the PaperPosition property of the current
%   figure so that the width of the figure is the fraction R of a
%   'uslsetter' page. It also sets the PaperSize property to tightly
%   match the figure size. In this way, printing to any format crops
%   the figure similar to what printing to EPS would do.
%
%   VL_PRINTSIZE(FIG,R) opearates on the specified figure FIG.
%
%   This command is useful to resize a figure before printing it so
%   that elements are scaled appropriately to match the desired figure
%   size in print. The function accepts the following optional
%   arguments:
%
%   Aspect:: [none]
%     Change the figure aspect ratio (widht/height) to the specified
%     value.
%
%   Reference:: [vertical]
%     If set to 'horizontal' the ratio R is the widht of the figure
%     over the width of the page. If 'vertical', the ratio R is the
%     height of the figure over the height of the page.
%
%   PaperType:: [letter]
%     Set the type (size) of the reference paperReference. Any of the
%     paper types supported by MATLAB can be used (see PRINT())).
%
%   See also: VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).

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
margin = 0 ;

for ai=1:2:length(varargin)
  opt = lower(varargin{ai}) ;
  arg = varargin{ai+1} ;
  switch lower(opt)
    case 'aspect'
      aspectRatio = arg ;
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
    case 'margin'
      margin = arg ;
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

% resize the figure
switch reference
  case 'horizontal'
    s = paperSize(1) / position(3) * sizeRatio ;
  case 'vertical'
    s = paperSize(2) / position(4) * sizeRatio ;
end
position(3:4) = position(3) * s * [1 1/aspectRatio] ;

% add margin
switch reference
  case 'horizontal'
    position(1) = position(3) * margin ;
    position(2) = position(3) * margin ;
  case 'vertical'
    position(1) = position(4) * margin ;
    position(2) = position(4) * margin ;
end

set(fig, 'PaperPosition', position, ...
         'PaperSize', 2 * position(1:2) + position(3:4)) ;
end
