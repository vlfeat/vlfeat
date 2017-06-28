#!/bin/sh

TESTS_PATH=./bin/glnxa64
TESTS_LIST="
aib
mser
sift
test_gauss_elimination
test_getopt_long
test_gmm
test_heap-def
test_host
test_imopv
test_kmeans
test_liop
test_mathop
test_mathop_abs
test_nan
test_qsort-def
test_rand
test_sqrti
test_stringop
test_svd2
test_threads
test_vec_comp
"

RET="OK"
for TEST in $TESTS_LIST
do
    echo "Running $TEST ..."
    $TESTS_PATH/$TEST || RET="FAIL"
done

if [ $RET != "OK" ] ; then
    echo "Some tests are failed"
    exit 1
fi
