% VL_TWISTER Random number generator
%    VL_TWISTER() is essentially equivalent to MATLAB native RAND()
%    when using the Twister random number generator. VL_TWISTER(),
%    VL_TWISTER(M,N,P,...)  and VL_TWISTER([M N P ...]) are equivalent
%    to RAND(), RAND(M,N,P,...)  and RAND([M N P ...]) respectively.
%
%    The state of the random generator can be seeded by
%    VL_TWISTER('STATE', X), where X is a DOUBLE scalar (this is
%    equivalent to RAND('TWISTER', X)). The state can be read by
%    VL_TWISTER('STATE') (equivalent to RAND('TWISTER')) and set by
%    VL_TWISTER('STATE', STATE) (equivalent to RAND('TWISTER',
%    STATE)). Here STATE is a vector of 625 elements of class
%    UINT32. Finally VL_TWISTER('STATE',KEY) seeds the generator by a
%    vector of DOUBLE of length not greater than 624.
%
%    VL_TWISTER() is slightly faster than RAND(). Moreover it can be
%    used to control the state of the random number generator used by
%    all VLFEAT functions.
%
%    See also: VL_HELP().

% Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
% All rights reserved.
%
% This file is part of the VLFeat library and is made available under
% the terms of the BSD license (see the COPYING file).
