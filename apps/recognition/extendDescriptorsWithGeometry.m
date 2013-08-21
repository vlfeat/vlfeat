function descrs = extendDescriptorsWithGeometry(type, frames, descrs)
% EXTENDDESCRIPTORSWITHGEOMETRY  Extend feature descriptors with geometric components
%    DESCRS = EXTENDDESCRIPTORSWITHGEOMETRY(TYPE, FRAMES, DESCRS)
%    extends the descriptorss DESCRS with either nothing (TYPE =
%    'none') or XY (TYPE = 'xy') from the FRAMES matrix. Note that,
%    for this to make sense, DESCRS and FRAMES should be properly normalized.

% Author: Andrea Vedaldi

% Copyright (C) 2013 Andrea Vedaldi
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).


switch lower(type)
  case 'none'
  case 'xy'
    z = frames(1:2,:) ;
    descrs = cat(1, descrs, z) ;
  otherwise
    error('Uknown geometric extension ''%s''.', type) ;
end
