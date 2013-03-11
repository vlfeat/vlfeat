/** @file fisher.h
 ** @brief Fisher encoding (@ref fisher)
 ** @author Andrea Vedaldi
 ** @author David Novotny
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#ifndef VL_FISHER_H
#define VL_FISHER_H

#include "generic.h"

/** @brief Fisher multithreading setup */

typedef enum _VlFisherMultithreading
{
  VlFisherParallel,    /**< Compute using one thread */
  VlFisherSerial       /**< Turn multithreading on */
} VlFisherMultithreading ;


VL_EXPORT void
vl_fisher_encode
(vl_type dataType,
 void const * data,
 void const * means,
 void const * sigmas,
 void const * weights,
 void * enc,
 vl_size dimension,
 vl_size numData,
 vl_size numClusters,
 VlFisherMultithreading multithreading);


#endif
