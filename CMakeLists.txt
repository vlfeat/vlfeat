cmake_minimum_required (VERSION 3.10)
project (vlfeat)

# make sure that the default is a RELEASE
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(Matlab COMPONENTS MX_LIBRARY REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D__LITTLE_ENDIAN__)
  add_definitions(-D__SSE2__)
  add_definitions(/Zp8)
  if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  add_definitions(-std=c99)
  add_definitions(-Wno-unused-function)
  add_definitions(-Wno-long-long)
  add_definitions(-Wno-variadic-macros)
endif()


set(SSE2_VL_FILES "vl/mathop_sse2.c" "vl/imopv_sse2.c")
set(AVX_VL_FILES "vl/mathop_avx.c")
add_definitions(-DVL_DISABLE_AVX)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/toolbox)

add_library(vl SHARED
  vl/aib.c
  vl/array.c
  vl/covdet.c
  vl/dsift.c
  vl/fisher.c
  vl/generic.c
  vl/getopt_long.c
  vl/gmm.c
  vl/hikmeans.c
  vl/hog.c
  vl/homkermap.c
  vl/host.c
  vl/ikmeans.c
  vl/imopv.c
  vl/kdtree.c
  vl/kmeans.c
  vl/lbp.c
  vl/liop.c
  vl/mathop.c
  ${AVX_VL_FILES}
  ${SSE2_VL_FILES}
  vl/mser.c
  vl/pgm.c
  vl/quickshift.c
  vl/random.c
  vl/rodrigues.c
  vl/scalespace.c
  vl/sift.c
  vl/slic.c
  vl/stringop.c
  vl/svm.c
  vl/svmdataset.c
  vl/vlad.c
)
set_property(TARGET vl PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(vl PRIVATE -DVL_BUILD_DLL)

list(APPEND vl_files
vl_aibhist        toolbox/aib/vl_aibhist.c
vl_aib            toolbox/aib/vl_aib.c
vl_alldist2       toolbox/misc/vl_alldist2.c
vl_alldist        toolbox/misc/vl_alldist.c
vl_binsearch      toolbox/misc/vl_binsearch.c
vl_binsum         toolbox/misc/vl_binsum.c
vl_covdet         toolbox/sift/vl_covdet.c
vl_cummax         toolbox/misc/vl_cummax.c
vl_dsift          toolbox/sift/vl_dsift.c
vl_erfill         toolbox/mser/vl_erfill.c
vl_fisher         toolbox/fisher/vl_fisher.c
vl_getpid         toolbox/misc/vl_getpid.c
vl_gmm            toolbox/gmm/vl_gmm.c
vl_hikmeanspush   toolbox/kmeans/vl_hikmeanspush.c
vl_hikmeans       toolbox/kmeans/vl_hikmeans.c
vl_hog            toolbox/misc/vl_hog.c
vl_homkermap      toolbox/misc/vl_homkermap.c
vl_ihashfind      toolbox/misc/vl_ihashfind.c
vl_ihashsum       toolbox/misc/vl_ihashsum.c
vl_ikmeanspush    toolbox/kmeans/vl_ikmeanspush.c
vl_ikmeans        toolbox/kmeans/vl_ikmeans.c
vl_imdisttf       toolbox/imop/vl_imdisttf.c
vl_imintegral     toolbox/imop/vl_imintegral.c
vl_imsmooth       toolbox/imop/vl_imsmooth.c
vl_imwbackwardmx  toolbox/imop/vl_imwbackwardmx.c
vl_inthist        toolbox/misc/vl_inthist.c
vl_irodr          toolbox/geometry/vl_irodr.c
vl_kdtreebuild    toolbox/misc/vl_kdtreebuild.c
vl_kdtreequery    toolbox/misc/vl_kdtreequery.c
vl_kmeans         toolbox/kmeans/vl_kmeans.c
vl_lbp            toolbox/misc/vl_lbp.c
vl_liop           toolbox/sift/vl_liop.c
vl_localmax       toolbox/misc/vl_localmax.c
vl_mser           toolbox/mser/vl_mser.c
vl_quickshift     toolbox/quickshift/vl_quickshift.c
vl_rodr           toolbox/geometry/vl_rodr.c
vl_sampleinthist  toolbox/misc/vl_sampleinthist.c
vl_siftdescriptor toolbox/sift/vl_siftdescriptor.c
vl_sift           toolbox/sift/vl_sift.c
vl_simdctrl       toolbox/misc/vl_simdctrl.c
vl_slic           toolbox/slic/vl_slic.c
vl_svmtrain       toolbox/misc/vl_svmtrain.c
vl_threads        toolbox/misc/vl_threads.c
vl_tpsumx         toolbox/imop/vl_tpsumx.c
vl_twister        toolbox/misc/vl_twister.c
vl_ubcmatch       toolbox/sift/vl_ubcmatch.c
vl_version        toolbox/misc/vl_version.c
vl_vlad           toolbox/vlad/vl_vlad.c
)

list(LENGTH vl_files size)
math(EXPR size "${size} - 1")
foreach(val RANGE 0 ${size} 2)
  math(EXPR nextval "${val} + 1")
  list(GET vl_files ${val} mex)
  list(GET vl_files ${nextval} src)
  matlab_add_mex(NAME ${mex} SRC ${src} LINK_TO vl)
  install(TARGETS ${mex} DESTINATION ${CMAKE_SOURCE_DIR}/toolbox/mex/${Matlab_MEX_EXTENSION}/)
endforeach()

install(TARGETS vl DESTINATION ${CMAKE_SOURCE_DIR}/bin/${Matlab_MEX_EXTENSION}/)
install(TARGETS vl DESTINATION ${CMAKE_SOURCE_DIR}/toolbox/mex/${Matlab_MEX_EXTENSION}/)
