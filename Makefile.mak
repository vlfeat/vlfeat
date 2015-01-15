# file: Makefile.mak
# descrption: Microsoft NMake makefile
# authors: Andrea Vedaldi, Brian Fulkerson, Mircea Cimpoi

# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

# --------------------------------------------------------------------
#                                                        Customization
# --------------------------------------------------------------------
# To modify this script to run on your platform it is usually
# sufficient to modify the following variables:
#
# ARCH: Either win32 or win64 [win64]
# DEBUG: Set to yes to ativate debugging [no]
# MATLABROOT: Path to MATLAB
# MSVSVER: Visual Studio version (e.g. 80, 90, 100) [90 for VS 9.0]
# MSVCROOT: Visual C++ location [$(VCInstallDir)].
# WINSDKROOT: Windows SDK location [$(WindowsSdkDir)]
#
# Note that some of these variables depend on the architecture
# (either win32 or win64).

VER = 0.9.20
ARCH = win64
DEBUG = no
BRANCH = v$(VER)-$(ARCH)
MSVSVER =
MSVCROOT = $(VCINSTALLDIR)
WINSDKROOT = $(WINDOWSSDKDIR)
GIT = git

!if "$(MSVCROOT)" == ""
MSVCROOT = C:\Program Files\Microsoft Visual Studio 10.0\VC
!endif

!if "$(WINSDKROOT)" == ""
WINSDKROOT = C:\Program Files\Microsoft SDKs\Windows\v7.0A
!endif

!include make/nmake_helper.mak

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 32-bit
!if "$(ARCH)" == "win32"
!message === COMPILING FOR 32-BIT

MATLABROOT = C:\Program Files (x86)\MATLAB\R2010b
MEX = "$(MATLABROOT)\bin\mex.bat"
MEXOPT = "$(MATLABROOT)\bin\win32\mexopts\msvc$(MSVSVER)opts.bat"
MEXEXT = mexw32
MEX_FLAGS =

CC = "$(MSVCROOT)\bin\cl.exe"
LINK = "$(MSVCROOT)\bin\link.exe"
MSVCR_PATH = $(MSVCROOT)\redist\x86\Microsoft.VC$(MSVSVER).CRT

LFLAGS = /MACHINE:X86 \
         /LIBPATH:"$(MSVCROOT)\lib" \
         /LIBPATH:"$(WINSDKROOT)\lib"

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 64-bit
!elseif "$(ARCH)" == "win64"
!message === COMPILING FOR 64-BIT

MATLABROOT = C:\Program Files\MATLAB\R2010b
MEX = "$(MATLABROOT)\bin\mex.bat"
MEXOPT = "$(MATLABROOT)\bin\win64\mexopts\msvc$(MSVSVER)opts.bat"
MEXEXT = mexw64
MEX_FLAGS = -largeArrayDims

CC = "$(MSVCROOT)\bin\amd64\cl.exe"
LINK = "$(MSVCROOT)\bin\amd64\link.exe"
!if $(MSVSVER) >= 100
MSVCR_PATH = $(MSVCROOT)\redist\x64\Microsoft.VC$(MSVSVER).CRT
!else
MSVCR_PATH = $(MSVCROOT)\redist\amd64\Microsoft.VC$(MSVSVER).CRT
!endif

LFLAGS = /MACHINE:X64 \
         /LIBPATH:"$(MSVCROOT)\lib\amd64" \
         /LIBPATH:"$(WINSDKROOT)\lib\x64"
!else
!error ARCH = $(ARCH) is an unknown architecture.
!endif

# --------------------------------------------------------------------
#                                                                Flags
# --------------------------------------------------------------------
# Debug info is embedded in .obj and .lib files (CodeView /Z7 option)
# but in .pdb files for .exe and .dll (since the linker does not
# produce CodeView output anymore).
#
# CFLAGS
#   /nologo            : CL does not display splash
#   _CRT_NO_DEPRECATE  : Do not deprecate `insecure' fscanf, snprintf, ...
#   __LITTLE_ENDIAN__  : Signal little endian architecture
#   /I.                : Add VLROOT to include search path
#   /MD                : Multi-thread run-time library dynamically linked
#   /TC                : Source code is C (not C++)
#   /W3                : Usa all warnings
#   /Zp8               : Align structures to 8 bytes
#   /Ox                : Turn on optimizations
#   /D"DEBUG"          : [DEBUG] Turn on debugging in VLFeat
#   /Z7                : [DEBUG] Embedded CodeView debug info in .obj
#   /D"NDEBUG"         : [NO DEBUG] Switches off asserts
#
# LFLAGS
#   /NOLOGO            : LINK does not display splash
#   /INCREMENTAL:NO    : No incremental linking
#   /MANIFEST          : See DLL HELL below
#   /DEBUG             : [DEBUG] Generate debug info (.pdb files)
#
# MEX_FLAGS
#   -I                 : Include VLFeat
#   -L                 : Add a library search path
#   -l                 : Link a dll
#
# ======================= ABOUT THE DLL HELL =========================
#
# This makefile compiles VLFeat to make use of the side-by-side
# deployment model, redestribtin the appropraite Visual C runtime
# library with the library and executables. In Visual Studio < 10.0
# this meant including a manifest file, while in version >= 10.0 this
# requirement has been relaxed.
#
# References:
#   http://www.codeguru.com/forum/showthread.php?t=408061
#   http://mariusbancila.ro/blog/2010/03/24/visual-studio-2010-changes-for-vc-part-5
#   http://social.msdn.microsoft.com/Forums/is/vcgeneral/thread/ca9177b2-2d02-42d8-8892-c6a25e6cfadb
#

bindir = bin\$(ARCH)
mexdir = toolbox\mex\$(MEXEXT)
objdir = $(bindir)\objs

CFLAGS = /nologo /TC /MD \
         /D"_CRT_SECURE_NO_DEPRECATE" \
         /D"__LITTLE_ENDIAN__" \
         /D"VL_DISABLE_AVX" \
         /I. \
         /W1 /Zp8 /openmp

LFLAGS = $(LFLAGS) /NOLOGO \
         /INCREMENTAL:NO \
         /MANIFEST

!if "$(DEBUG)" != "no"
!message === DEBUGGING ON
CFLAGS = $(CFLAGS) /Z7 /D"DEBUG"
LFLAGS = $(LFLAGS) /DEBUG
MEX_FLAGS = $(MEX_FLAGS) -g
!else
!message === DEBUGGING OFF
CFLAGS = $(CFLAGS) /D"NDEBUG" /Ox
!endif

DLL_CFLAGS = /D"VL_BUILD_DLL"
EXE_LFLAGS = $(LFLAGS) /LIBPATH:"$(bindir)" vl.lib
MEX_FLAGS = $(MEX_FLAGS) -f $(MEXOPT) -I. -Itoolbox -L"$(bindir)" -lvl

libsrc = \
  vl\aib.c \
  vl\array.c \
  vl\covdet.c \
  vl\dsift.c \
  vl\fisher.c \
  vl\generic.c \
  vl\getopt_long.c \
  vl\gmm.c \
  vl\hikmeans.c \
  vl\hog.c \
  vl\homkermap.c \
  vl\host.c \
  vl\ikmeans.c \
  vl\imopv.c \
  vl\imopv_sse2.c \
  vl\kdtree.c \
  vl\kmeans.c \
  vl\lbp.c \
  vl\liop.c \
  vl\mathop.c \
  vl\mathop_avx.c \
  vl\mathop_sse2.c \
  vl\mser.c \
  vl\pgm.c \
  vl\quickshift.c \
  vl\random.c \
  vl\rodrigues.c \
  vl\scalespace.c \
  vl\sift.c \
  vl\slic.c \
  vl\stringop.c \
  vl\svm.c \
  vl\svmdataset.c \
  vl\vlad.c

cmdsrc = \
  src\aib.c \
  src\mser.c \
  src\sift.c \
  src\test_gauss_elimination.c \
  src\test_getopt_long.c \
  src\test_gmm.c \
  src\test_heap-def.c \
  src\test_host.c \
  src\test_imopv.c \
  src\test_kmeans.c \
  src\test_liop.c \
  src\test_mathop.c \
  src\test_mathop_abs.c \
  src\test_nan.c \
  src\test_qsort-def.c \
  src\test_rand.c \
  src\test_sqrti.c \
  src\test_stringop.c \
  src\test_svd2.c \
  src\test_threads.c \
  src\test_vec_comp.c

cmdsrc = \
  src\aib.c \
  src\mser.c \
  src\sift.c \
  src\test_gauss_elimination.c \
  src\test_getopt_long.c \
  src\test_gmm.c \
  src\test_heap-def.c \
  src\test_host.c \
  src\test_imopv.c \
  src\test_kmeans.c \
  src\test_liop.c \
  src\test_mathop.c \
  src\test_mathop_abs.c \
  src\test_nan.c \
  src\test_qsort-def.c \
  src\test_rand.c \
  src\test_sqrti.c \
  src\test_stringop.c \
  src\test_svd2.c \
  src\test_threads.c \
  src\test_vec_comp.c

mexsrc = \
  toolbox\aib\vl_aib.c \
  toolbox\aib\vl_aibhist.c \
  toolbox\fisher\vl_fisher.c \
  toolbox\geometry\vl_irodr.c \
  toolbox\geometry\vl_rodr.c \
  toolbox\gmm\vl_gmm.c \
  toolbox\imop\vl_imdisttf.c \
  toolbox\imop\vl_imintegral.c \
  toolbox\imop\vl_imsmooth.c \
  toolbox\imop\vl_imwbackwardmx.c \
  toolbox\imop\vl_tpsumx.c \
  toolbox\kmeans\vl_hikmeans.c \
  toolbox\kmeans\vl_hikmeanspush.c \
  toolbox\kmeans\vl_ikmeans.c \
  toolbox\kmeans\vl_ikmeanspush.c \
  toolbox\kmeans\vl_kmeans.c \
  toolbox\misc\vl_alldist.c \
  toolbox\misc\vl_alldist2.c \
  toolbox\misc\vl_binsearch.c \
  toolbox\misc\vl_binsum.c \
  toolbox\misc\vl_cummax.c \
  toolbox\misc\vl_getpid.c \
  toolbox\misc\vl_hog.c \
  toolbox\misc\vl_homkermap.c \
  toolbox\misc\vl_ihashfind.c \
  toolbox\misc\vl_ihashsum.c \
  toolbox\misc\vl_inthist.c \
  toolbox\misc\vl_kdtreebuild.c \
  toolbox\misc\vl_kdtreequery.c \
  toolbox\misc\vl_lbp.c \
  toolbox\misc\vl_localmax.c \
  toolbox\misc\vl_sampleinthist.c \
  toolbox\misc\vl_simdctrl.c \
  toolbox\misc\vl_svmtrain.c \
  toolbox\misc\vl_threads.c \
  toolbox\misc\vl_twister.c \
  toolbox\misc\vl_version.c \
  toolbox\mser\vl_erfill.c \
  toolbox\mser\vl_mser.c \
  toolbox\quickshift\vl_quickshift.c \
  toolbox\sift\vl_covdet.c \
  toolbox\sift\vl_dsift.c \
  toolbox\sift\vl_liop.c \
  toolbox\sift\vl_sift.c \
  toolbox\sift\vl_siftdescriptor.c \
  toolbox\sift\vl_ubcmatch.c \
  toolbox\slic\vl_slic.c \
  toolbox\vlad\vl_vlad.c

!if "$(ARCH)" == "win32"
libobj = $(libsrc:vl\=bin\win32\objs\)
cmdexe = $(cmdsrc:src\=bin\win32\)
mexdll = $(mexsrc:.c=.mexw32)
mexdll = $(mexdll:toolbox\fisher=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\sift=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\mser=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\imop=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\geometry=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\gmm=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\misc=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\aib=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\quickshift=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\slic=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\vlad=toolbox\mex\mexw32)
mexpdb = $(mexdll:.dll=.pdb)

!elseif "$(ARCH)" == "win64"
libobj = $(libsrc:vl\=bin\win64\objs\)
cmdexe = $(cmdsrc:src\=bin\win64\)
mexdll = $(mexsrc:.c=.mexw64)
mexdll = $(mexdll:toolbox\fisher=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\sift=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\mser=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\imop=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\geometry=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\gmm=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\misc=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\aib=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\quickshift=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\slic=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\vlad=toolbox\mex\mexw64)
mexpdb = $(mexdll:.mexw64=.pdb)
!endif

libobj = $(libobj:.c=.obj)
cmdexe = $(cmdexe:.c=.exe)
cmdpdb = $(cmdexe:.exe=.pdb)

# Visual Studio redistributable files
MSVCR = Microsoft.VC$(MSVSVER).CRT
!if $(MSVSVER) <= 90
# VS <= 2008 needs a manifest too
bincrt = $(bindir)\msvcr$(MSVSVER).dll $(bindir)\$(MSVCR).manifest
mexcrt = $(mexdir)\msvcr$(MSVSVER).dll $(mexdir)\$(MSVCR).manifest
!else
bincrt = $(bindir)\msvcr$(MSVSVER).dll
mexcrt = $(mexdir)\msvcr$(MSVSVER).dll
!endif

!ifdef MATLABROOT
all: $(bindir) $(objdir) $(mexdir) \
     $(bincrt) $(bindir)\vl.lib $(bindir)\vl.dll \
     $(cmdexe) \
     $(mexcrt) $(mexdir)\vl.dll $(mexdll)
!else
all: $(bindir) $(objdir) \
     $(bincrt) $(bindir)\vl.lib $(bindir)\vl.dll \
     $(cmdexe)
!endif

BUILD_MEX=@echo .... CC [MEX] $(@) && \
	$(MEX) $(MEX_FLAGS) "$(<)" -output $(@)

# --------------------------------------------------------------------
#                                                    Maintenance rules
# --------------------------------------------------------------------

clean:
	del /f /Q $(libobj)
	del /f /Q $(objdir)
	del /f /Q $(cmdpdb)
	del /f /Q $(mexpdb)

archclean:
	if exist bin\$(ARCH) rmdir /S /Q bin\$(ARCH)
	if exist $(mexdir) rmdir /S /Q $(mexdir)

distclean:
	if exist bin rmdir /S /Q bin
	if exist toolbox\mex rmdir /S /Q toolbox\mex

info:
	@echo $(mexx)
	@echo ** bindir      = $(bindir)
	@echo ** mexdir      = $(mexdir)
	@echo ** objdir      = $(objdir)
	@echo ** libsrc      = $(libsrc)
	@echo ** libobj      = $(libobj)
	@echo ** cmdsrc      = $(cmdsrc)
	@echo ** cmdexe      = $(cmdexe)
	@echo ** mexsrc      = $(mexsrc)
	@echo ** mexdll      = $(mexdll)
	@echo ** CC          = $(CC)
	@echo ** CFLAGS      = $(CFLAGS)
	@echo ** DLL_CFLAGS  = $(DLL_CFLAGS)
	@echo ** MEX_FLAGS   = $(MEX_FLAGS)
	@echo ** BUILD_MEX   = "$(BUILD_MEX)"
	@echo ** MATLABROOT  = $(MATLABROOT)
	@echo ** MEX         = $(MEX)
	@echo ** MEXEXT      = $(MEXEXT)
	@echo ** MEXOPT      = $(MEXOPT)
	@echo ** MSVSVER     = $(MSVSVER)
	@echo ** MSVCROOT    = $(MSVCROOT)
	@echo ** MSVCR       = $(MSVCR)
	@echo ** MSVCR_PATH  = $(MSVCR_PATH)
	@echo ** bincrt      = $(bincrt)
	@echo ** mexcrt      = $(mexcrt)
	@echo ** WINSDKROOT  = $(WINSDKROOT)
	@echo ** DEBUG       = $(DEBUG)

# --------------------------------------------------------------------
#                                                          Build rules
# --------------------------------------------------------------------

# create directory if missing
$(bindir) :
	mkdir $(bindir)

$(objdir) :
	mkdir $(objdir)

$(mexdir) :
	mkdir $(mexdir)

# --------------------------------------------------------------------
#                                      Rules to compile the VLFeat DLL
# --------------------------------------------------------------------

# special sources with SSE2 support
$(objdir)\mathop_sse2.obj : vl\mathop_sse2.c
	@echo .... CC [+SSE2] $(@)
	@$(CC) $(CFLAGS) $(DLL_CFLAGS) /arch:SSE2 /D"__SSE2__" /c /Fo"$(@)" "vl\$(@B).c"

$(objdir)\imopv_sse2.obj : vl\imopv_sse2.c
	@echo .... CC [+SSE2] $(@)
	@$(CC) $(CFLAGS) $(DLL_CFLAGS) /arch:SSE2 /D"__SSE2__" /c /Fo"$(@)" "vl\$(@B).c"

# vl\*.c -> $objdir\*.obj
{vl}.c{$(objdir)}.obj:
	@echo .... CC $(@)
	@$(CC) $(CFLAGS) $(DLL_CFLAGS) /c /Fo"$(@)" "$(<)"

# Link VLFeat DLL
$(bindir)\vl.dll : $(libobj)
	@echo .. LINK [DLL] $(@R).dll
	$(LINK) /DLL  $(LFLAGS) $(**) /OUT:"$(@)"
	@mt /nologo /outputresource:"$(@);#2" /manifest "$(@R).dll.manifest"
	@-del "$(@R).dll.manifest"

# *.obj -> *.lib
$(bindir)\vl.lib : $(libobj)
	@echo ... LIB $(@R).lib
	@lib $(**) /OUT:"$(@)" /NOLOGO

# redistributable: msvcr__.dll => bin/win{32,64}/msvcr__.dll
$(bindir)\$(MSVCR).manifest : "$(MSVCR_PATH)\$(MSVCR).manifest"
        copy $(**) "$(@)"

$(bindir)\msvcr$(MSVSVER).dll: "$(MSVCR_PATH)\msvcr$(MSVSVER).dll"
        copy $(**) "$(@)"

# --------------------------------------------------------------------
#                                Rules to compile the VLFeat EXE files
# --------------------------------------------------------------------

# src\*.c -> $bindir\*.exe
{src}.c{$(bindir)}.exe:
	@echo .... CC [EXE] $(@)
	@$(CC) $(CFLAGS) /Fe"$(@)" /Fo"$(@R).obj" "$(<)" /link $(EXE_LFLAGS)
	@MT /nologo /outputresource:"$(@);#1" /manifest "$(@).manifest"
	@-del "$(@).manifest"
	@-del "$(@R).obj"

# --------------------------------------------------------------------
#                                Rules to compile the VLFeat MEX files
# --------------------------------------------------------------------

startmatlab:
	"$(MATLABROOT)/bin/matlab.exe" -nodesktop

# toolbox\*.c -> toolbox\*.dll
{toolbox\sift}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\mser}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\imop}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\gmm}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\geometry}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\kmeans}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\aib}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\quickshift}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\misc}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\slic}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\vlad}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

{toolbox\fisher}.c{$(mexdir)}.$(MEXEXT):
	$(BUILD_MEX)

# vl.dll => mexw{32,64}\vl.dll
$(mexdir)\vl.dll : $(bindir)\vl.dll
	copy "$(**)" "$(@)"

# Ideally, the DLL should be linked to Intel compatibility library libiomp5md.dll that
# ships with MATLAB. However, there does not seem to be a clean way to do so without
# the .lib file. This is suboptimal as it casues two OMP libraries to be used (vcomp and iomp5).
# Possible work arounds that did not work yet: generate the .lib file from the .dll file,
# redirect somehow vcomp to iomp5.

#$(LINK) /LIBPATH:"$(MATLABROOT)\extern\lib\win64\microsoft" /DLL $(LFLAGS) $(**) libmwblas.lib /nodefaultlib:vcomp /OUT:"$(@)"
#$(mexdir)\vl.dll : $(libobj)
#	@echo .. LINK [DLL] $(@R).dll
#  $(LINK) /DLL $(LFLAGS) $(**) /OUT:"$(@)"
#	@-del "$(@R).dll.manifest"

# redistributable: msvcr__.dll => bin/win{32,64}/msvcr__.dll
$(mexdir)\$(MSVCR).manifest : "$(MSVCR_PATH)\$(MSVCR).manifest"
        copy $(**) "$(@)"

$(mexdir)\msvcr$(MSVSVER).dll: "$(MSVCR_PATH)\msvcr$(MSVSVER).dll"
        copy $(**) "$(@)"

# --------------------------------------------------------------------
#                                       Rules to post the binary files
# --------------------------------------------------------------------

bin-release:
	echo Fetching remote tags && \
	$(GIT) fetch --tags && \
	echo Checking out v$(VER) && \
	$(GIT) checkout v$(VER)
	echo Rebuilding binaries for release
	if exist "bin\$(ARCH)" del /f /Q "bin\$(ARCH)"
	if exist "bin\mex\$(ARCH)" del /f /Q "toolbox\mex$(ARCH)"
	nmake /f Makefile.mak ARCH=$(ARCH)

bin-commit: bin-release
	@echo Fetching remote tags && \
	$(GIT) fetch --tags
	@echo Crearing/resetting and checking out branch $(BRANCH) to v$(VER) && \
	$(GIT) branch -f $(BRANCH) v$(VER) && \
	$(GIT) checkout $(BRANCH)
	@echo Adding binaries && \
	$(GIT) add -f $(bincrt) && \
	$(GIT) add -f "$(bindir)\vl.lib" && \
	$(GIT) add -f "$(bindir)\vl.dll" && \
	$(GIT) add -f $(cmdexe) && \
	@echo Adding MEX files && \
	$(GIT) add -f $(mexcrt) && \
	$(GIT) add -f "$(mexdir)\vl.dll" && \
	$(GIT) add -f $(mexdll) && \
	@echo Commiting changes && \
	$(GIT) commit -m "$(ARCH) binaries for version $(VER)"
	@echo Commiting and pushing to server the binaries && \
	$(GIT) push -v --force bin $(BRANCH):refs/heads/$(BRANCH) && \
	$(GIT) checkout v$(VER) && \
	$(GIT) branch -D $(BRANCH)
