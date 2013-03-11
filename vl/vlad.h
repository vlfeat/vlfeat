/** @file vlad.h
 ** @brief VLAD encoding (@ref vlad)
 ** @author Andrea Vedaldi
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_VLAD_H
#define VL_VLAD_H

#include "generic.h"

VL_EXPORT void
vl_vlad_encode
(vl_type dataType,
 void const * data,
 void const * means,
 void const * assignments,
 void * enc,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters,
 vl_bool normalize);


#endif
