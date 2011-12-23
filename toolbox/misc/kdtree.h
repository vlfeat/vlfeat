/** @internal
 ** @file     KDForest.c
 ** @brief    KDForest MEX utilities
 ** @author   Andrea Vedaldi
 **/

#include "mex.h"
#include <mexutils.h>
#include <vl/kdtree.h>

/** ------------------------------------------------------------------
 ** @interan@brief Hepler function
 ** @param tree KDForest object to process
 ** @param nodeIndex index of the KDForest node to start from
 ** @param numNodesToVisit total number of nodes to visit.
 **
 ** The function visits in depth first order the tree's nodes starting
 ** from the root, restoring the node parent pointers.
 **
 ** It also chekcs for tree consistency, aborting the MEX file in case
 ** of inconsistencies. Loops are detected by counting how many nodes
 ** have been visited so far compared to the total number of nodes in
 ** the tree.
 **/

void
restore_parent_recursively (VlKDTree * tree, int nodeIndex, int * numNodesToVisit)
{
  VlKDTreeNode * node = tree->nodes + nodeIndex ;
  int lowerChild = node->lowerChild ;
  int upperChild = node->upperChild ;

  if (*numNodesToVisit == 0) {
    vlmxError (vlmxErrInconsistentData,
              "FOREST.TREES has an inconsitsent tree structure.") ;
  }

  *numNodesToVisit -= 1 ;

  if (lowerChild >= 0) {
    VlKDTreeNode * child = tree->nodes + lowerChild ;
    child->parent = nodeIndex ;
    restore_parent_recursively (tree, lowerChild, numNodesToVisit) ;
  }
  if (upperChild >= 0) {
    VlKDTreeNode * child = tree->nodes + upperChild ;
    child->parent = nodeIndex ;
    restore_parent_recursively (tree, upperChild, numNodesToVisit) ;
  }
}

/** ------------------------------------------------------------------
 ** @internal @brief Builds a MEX array representing a VlKDForest object
 ** @param tree object to convert.
 ** @return MEX representation of the tree.
 **
 ** The KDForest object returned encapsulates the data (no copies are made).
 ** Recall that a KDForest object by design does not own the data.
 **
 ** In case of error, the function aborts by calling ::mxErrMsgTxt.
 **/

static mxArray *
new_array_from_kdforest (VlKDForest const * forest)
{
  vl_uindex ti ;
  mwSize dims [] = {1,1} ;
  mwSize treeDims [] = {1,0} ;
  char const * fieldNames [] = {
    "dimension",
    "numData",
    "trees"
  } ;
  char const * treeFieldNames [] = {
    "nodes",
    "dataIndex"
  } ;
  char const * nodesFieldNames [] = {
    "lowerChild",
    "upperChild",
    "splitDimension",
    "splitThreshold"
  } ;
  mxArray * forest_array ;
  mxArray * trees_array ;

  treeDims [0] = 1 ;
  treeDims [1] = forest->numTrees ;
  trees_array = mxCreateStructArray (2, treeDims,
                                     sizeof(treeFieldNames) / sizeof(treeFieldNames[0]),
                                     treeFieldNames) ;

  /*
    FOREST.NUMDIMENSIONS
    FOREST.NUMDATA
   */

  forest_array = mxCreateStructArray (2, dims, sizeof(fieldNames) / sizeof(fieldNames[0]), fieldNames) ;
  mxSetField (forest_array, 0, "dimension", vlmxCreatePlainScalar (forest->dimension)) ;
  mxSetField (forest_array, 0, "numData", vlmxCreatePlainScalar (forest->numData)) ;
  mxSetField (forest_array, 0, "trees", trees_array) ;

  for (ti = 0 ; ti < forest->numTrees ; ++ ti) {
    VlKDTree * tree = forest->trees[ti] ;
    mxArray * nodes_array = mxCreateStructArray (2, dims, sizeof(nodesFieldNames) / sizeof(nodesFieldNames[0]), nodesFieldNames) ;
    mxArray * dataIndex_array = mxCreateNumericMatrix (1, forest->numData, mxUINT32_CLASS, mxREAL) ;
    mxSetField (trees_array, ti, "nodes", nodes_array) ;
    mxSetField (trees_array, ti, "dataIndex", dataIndex_array) ;

    /*
     FOREST.TREES.NODES.LOWERCHILD
     FOREST.TREES.NODES.UPPERCHILD
     FOREST.TREES.NODES.SPLITDIMENSION
     FOREST.TREES.NODES.SPLITTHRESHOLD
     */
    {
      mxArray * lowerChild_array = mxCreateNumericMatrix (1, tree->numUsedNodes, mxINT32_CLASS, mxREAL) ;
      mxArray * upperChild_array = mxCreateNumericMatrix (1, tree->numUsedNodes, mxINT32_CLASS, mxREAL) ;
      mxArray * splitDimension_array = mxCreateNumericMatrix (1, tree->numUsedNodes, mxUINT32_CLASS, mxREAL) ;
      mxArray * splitThreshold_array = mxCreateNumericMatrix (1, tree->numUsedNodes, mxDOUBLE_CLASS, mxREAL) ;

      vl_uint32 * upperChild = mxGetData (upperChild_array) ;
      vl_uint32 * lowerChild = mxGetData (lowerChild_array) ;
      vl_uint32 * splitDimension = mxGetData (splitDimension_array) ;
      double * splitThreshold = mxGetData (splitThreshold_array) ;
      vl_uindex ni ;

      for (ni = 0 ; ni < tree -> numUsedNodes ; ++ ni) {
        VlKDTreeNode const * node = tree -> nodes + ni ;
        int a = node->upperChild ;
        int b = node->lowerChild ;
        upperChild [ni] = (a>=0) ? a + 1 : a ;
        lowerChild [ni] = (b>=0) ? b + 1 : b ;
        splitDimension [ni] = node->splitDimension + 1 ;
        splitThreshold [ni] = node->splitThreshold ;
      }
      mxSetField (nodes_array, 0, "lowerChild", lowerChild_array) ;
      mxSetField (nodes_array, 0, "upperChild", upperChild_array) ;
      mxSetField (nodes_array, 0, "splitDimension", splitDimension_array) ;
      mxSetField (nodes_array, 0, "splitThreshold", splitThreshold_array) ;
    }

    /* FOREST.TREEE.DATAINDEX */
    {
      vl_uint32 * dataIndex = mxGetData (dataIndex_array) ;
      vl_uindex di ;
      for (di = 0 ; di < forest->numData ; ++ di) {
        dataIndex [di] = forest->trees[ti]->dataIndex[di].index + 1 ;
      }
    }
  }
  return forest_array  ;
}


/** ------------------------------------------------------------------
 ** @internal @brief Builds a VlKDForest from MEX parameters
 ** @param forest_array MEX array representing the kd-tree
 ** @param data_array MEX array representing the kd-tree data
 ** @return KDForest object instance.
 **
 ** The KDForest object returned encapsulates the data (no copies are made).
 ** Recall that a KDForest object by design does not own the data.
 **
 ** In case of error, the function aborts by calling ::mxErrMsgTxt.
 **/

static VlKDForest *
new_kdforest_from_array (mxArray const * forest_array, mxArray const * data_array)
{
  VlKDForest * forest ;
  mxArray const * dimension_array ;
  mxArray const * numData_array ;
  mxArray const * trees_array ;
  mxArray const * nodes_array ;
  mxArray const * dataIndex_array ;
  mxArray const * lowerChild_array ;
  mxArray const * upperChild_array ;
  mxArray const * splitDimension_array ;
  mxArray const * splitThreshold_array ;

  vl_int32 const * lowerChild ;
  vl_int32 const * upperChild ;
  vl_uint32 const * splitDimension ;
  double const * splitThreshold ;

  vl_uindex ti ;
  int unsigned dimension ;
  vl_size numData ;
  vl_size numUsedNodes ;
  vl_size numTrees ;

  vl_type dataType ;

  /*
    FOREST.NUMDIMENSIONS
    FOREST.NUMDATA
    FOREST.DATA
    FOREST.TREES
   */
  if (! mxIsStruct (forest_array) ||
      mxGetNumberOfElements (forest_array) != 1) {
    vlmxError (vlmxErrInconsistentData,
              "FOREST must be a 1 x 1 structure.") ;
  }
  dimension_array = mxGetField (forest_array, 0, "dimension") ;
  if (! dimension_array ||
      ! vlmxIsPlainScalar (dimension_array) ||
      (dimension = mxGetScalar (dimension_array)) < 1) {
    vlmxError(vlmxErrInconsistentData,
             "FOREST.NUMDIMENSIONS must be a poisitve integer.") ;
  }
  numData_array = mxGetField (forest_array, 0, "numData") ;
  if (! numData_array ||
      ! vlmxIsPlainScalar (numData_array) ||
      (numData = mxGetScalar (numData_array)) < 1) {
    vlmxError(vlmxErrInconsistentData,
             "FOREST.NUMDATA must be a poisitve integer.") ;
  }
  trees_array = mxGetField (forest_array, 0, "trees") ;
  if (! mxIsStruct (trees_array)) {
    vlmxError(vlmxErrInconsistentData,
             "FOREST.TREES must be a structure array.") ;
  }
  numTrees = mxGetNumberOfElements (trees_array) ;
  if (numTrees < 1) {
    vlmxError(vlmxErrInconsistentData,
             "FOREST.TREES must have at least one element.") ;
  }

  if (! vlmxIsMatrix (data_array, dimension, numData)) {
    vlmxError(vlmxErrInconsistentData,
             "DATA dimensions are not compatible with TREE.") ;
  }
  if (! vlmxIsReal (data_array)) {
    vlmxError(vlmxErrInvalidArgument,
             "DATA must be real.") ;
  }
  switch (mxGetClassID (data_array)) {
    case mxSINGLE_CLASS : dataType = VL_TYPE_FLOAT ; break ;
    case mxDOUBLE_CLASS : dataType = VL_TYPE_DOUBLE ; break ;
    default :
      vlmxError(vlmxErrInvalidArgument,
               "DATA must be either SINGLE or DOUBLE.") ;
  }

  forest = vl_kdforest_new (dataType, dimension, numTrees) ;
  forest->numData = numData ;
  forest->trees = vl_malloc (sizeof(VlKDTree*) * numTrees) ;
  forest->data = mxGetData (data_array) ;

  /*
   FOREST.TREES.NODES
   FOREST.TREES.DATAINDEX
   */
  for (ti = 0 ; ti < numTrees ; ++ ti) {
    VlKDTree * tree = vl_malloc (sizeof(VlKDTree)) ;
    nodes_array = mxGetField (trees_array, ti, "nodes") ;
    dataIndex_array = mxGetField (trees_array, ti, "dataIndex") ;

    if (! nodes_array ||
        ! mxIsStruct (nodes_array)) {
      vlmxError(vlmxErrInconsistentData,
               "FOREST.TREES(%d).NODES must be a struct array.", ti+1) ;
    }

    /*
     FOREST.TREES.NODES.LOWERCHILD
     FOREST.TREES.NODES.UPPERCHILD
     FOREST.TREES.NODES.SPLITTHRESHOLD
     FOREST.TREES.NODES.SPLITDIMENSION
     */

    lowerChild_array = mxGetField (nodes_array, 0, "lowerChild") ;
    upperChild_array = mxGetField (nodes_array, 0, "upperChild") ;
    splitDimension_array = mxGetField (nodes_array, 0, "splitDimension") ;
    splitThreshold_array = mxGetField (nodes_array, 0, "splitThreshold") ;

    numUsedNodes = mxGetN (lowerChild_array) ;

    if (! lowerChild_array ||
        ! vlmxIsMatrix (lowerChild_array, 1, numUsedNodes) ||
        mxGetClassID (lowerChild_array) != mxINT32_CLASS) {
      vlmxError(vlmxErrInconsistentData,
               "FOREST.TREES(%d).NODES.LOWERCHILD must be a 1 x NUMNODES INT32 array.",ti+1) ;
    }
    if (! upperChild_array ||
        ! vlmxIsMatrix (upperChild_array, 1, numUsedNodes) ||
        mxGetClassID (upperChild_array) != mxINT32_CLASS) {
      vlmxError(vlmxErrInconsistentData,
               "FOREST.TREES(%d).NODES.UPPERCHILD must be a 1 x NUMNODES INT32 array.",ti+1) ;
    }
    if (! splitDimension_array ||
        ! vlmxIsMatrix (splitDimension_array, 1, numUsedNodes) ||
        mxGetClassID (splitDimension_array) != mxUINT32_CLASS) {
      vlmxError(vlmxErrInconsistentData,
               "FOREST.TREES(%d).NODES.SPLITDIMENSION must be a 1 x NUMNODES UINT32 array",ti+1) ;
    }
    if (! splitThreshold_array ||
        ! vlmxIsMatrix (splitThreshold_array, 1, numUsedNodes) ||
        mxGetClassID (splitThreshold_array) != mxDOUBLE_CLASS) {
      vlmxError(vlmxErrInconsistentData,
               "FOREST.TREES(%d).NODES.SPLITTHRESHOLD must be a 1 x NUMNODES DOUBLE array",ti+1) ;
    }
    lowerChild = (vl_int32*) mxGetData (lowerChild_array) ;
    upperChild = (vl_int32*) mxGetData (upperChild_array) ;
    splitDimension = (vl_uint32*) mxGetData (splitDimension_array) ;
    splitThreshold = (double*) mxGetData (splitThreshold_array) ;

    if (! dataIndex_array ||
        ! vlmxIsMatrix (dataIndex_array, 1, numData) ||
        mxGetClassID (dataIndex_array) != mxUINT32_CLASS) {
      vlmxError(vlmxErrInconsistentData,
               "FOREST.TREES(%d).DATAINDEX must be a 1 x NUMDATA array of class UINT32.",ti+1) ;
    }

    tree->numAllocatedNodes = numUsedNodes ;
    tree->numUsedNodes = numUsedNodes ;
    tree->nodes = vl_malloc (sizeof(VlKDTreeNode) * numUsedNodes) ;
    tree->dataIndex = vl_malloc (sizeof(VlKDTreeDataIndexEntry) * numData) ;

    {
      vl_uindex ni ;
      for (ni = 0 ; ni < numUsedNodes ; ++ ni) {
        vl_int32 lc = lowerChild [ni] ;
        vl_int32 uc = upperChild [ni] ;
        vl_uint32 d = splitDimension [ni] ;

        if (uc < - (signed)numData - 1 || uc > (signed)numUsedNodes) {
          vlmxError (vlmxErrInconsistentData,
                    "TREE.NODES.UPPERCHILD(%d)=%d out of bounds",
                    ni+1,uc) ;
        }
        if (lc < - (signed)numData || lc > (signed)numUsedNodes) {
          vlmxError (vlmxErrInconsistentData,
                    "TREE.NODES.LOWERCHILD(%d)=%d out of bounds",
                    ni+1,lc) ;
        }
        if (d > dimension) {
          vlmxError (vlmxErrInconsistentData,
                    "TREE.NODES.SPLITDIMENSION(%d)=%d out of bounds",
                    ni+1,d) ;
        }

        tree->nodes[ni].parent = 0 ;
        tree->nodes[ni].upperChild = (uc >= 1) ? uc-1 : uc ;
        tree->nodes[ni].lowerChild = (lc >= 1) ? lc-1 : lc ;
        tree->nodes[ni].splitDimension = d - 1 ;
        tree->nodes[ni].splitThreshold = splitThreshold[ni] ;
      }
    }

    {
      vl_uindex di ;
      vl_uint32 * dataIndex = mxGetData (dataIndex_array) ;
      for (di = 0 ; di < numData ; ++ di) {
        tree->dataIndex[di].index = dataIndex [di] - 1 ;
      }
    }

    {
      int numNodesToVisit = tree->numUsedNodes ;
      restore_parent_recursively (tree, 0, &numNodesToVisit) ;
      if (numNodesToVisit != 0) {
        vlmxError (vlmxErrInconsistentData,
                  "TREE has an inconsitsent tree structure.") ;
      }
    }

    forest->trees[ti] = tree ;
  }
  return forest ;
}
