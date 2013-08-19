function descrs = extendDescriptorsWithGeometry(type, frames, descrs)
% EXTENDDESCRIPTORSWITHGEOMETRY  Extend feature descriptors with geometric components

switch lower(type)
  case 'none'
  case 'xy'
    z = frames(1:2,:) ;
    descrs = cat(1, descrs, z) ;
  otherwise
    error('Uknown geometric extension ''%s''.', type) ;
end
