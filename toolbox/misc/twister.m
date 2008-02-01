% TWISTER Random number generator
%    TWISTER() is essentially equivalent to MATALB native RAND() when
%    using the Twister random number generator. TWISTER(),
%    TWISTER(M,N,P,...)  and TWISTER([M N P ...]) are equivalent to
%    RAND(), RAND(M,N,P,...)  and RAND([M N P ...]) respectively.
%
%    The state of the random generator can be seeded by
%    TWISTER('STATE', X), where X is a DOUBLE scalar (this is
%    equivalent to RAND('TWISTER', X)). The state can be read by
%    TWISTER('STATE') (equivalent to RAND('TWISTER')) and set by
%    TWISTER('STATE', STATE) (equivalent to RAND('TWISTER',
%    STATE)). Here STATE is a vector of 625 elements of class UINT32.
%
%    TWISTER() is slightly faster than RAND(). Moreover it can be used
%    to control the state of the random number gnerator of VLFEAT
%    functions.

% AUTORIGHTS
