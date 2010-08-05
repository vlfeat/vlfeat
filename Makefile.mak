# file:       Makefile.mak
# authors:    Andrea Vedaldi and Brian Fulkerson
# descrption: Microsoft NMake makefile

# --------------------------------------------------------------------
#                                                        Customization
# --------------------------------------------------------------------
#
# - MSVCR    : the file name of msvcr__.dll for your compiler
# - MSVCRLOC : must point to the location of msvcr__.dll for your compiler
# - MATLABROOT : must point to MATLAB root directory (undef = no MEX support)

# Here is an example of how the variables might look with a different version
# of Visual Studio and an alternate location for Matlab
#MSVCR      = msvcr80.dll
#MSVCP      = msvcp80.dll
#MSVCM      = msvcm80.dll
#MSMANIFEST = Microsoft.VC80.CRT.manifest
#MSVCRLOC   = C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT
#MATLABROOT = C:\Program Files\MATLAB08a

VER  = 0.9.9
ARCH = w32

VCROOT     = C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC
WINSDKROOT = C:\Program Files\Microsoft SDKs\Windows\v6.0
MATLABROOT = C:\Program Files (x86)\MATLAB\R2009b

CC         = "$(VCROOT)\bin\cl.exe"
LINK       = "$(VCROOT)\bin\link.exe"
MSVCRLOC   = $(VCROOT)\redist\x86\Microsoft.VC90.CRT
MSVCR      = msvcr90.dll
MSVCP      = msvcp90.dll
MSVCM      = msvcm90.dll
MSMANIFEST = Microsoft.VC90.CRT.manifest

MEX        = "$(MATLABROOT)\bin\mex.bat"
MEXEXT     = mexw32
MEXOPT     = $(MATLABROOT)\bin\win32\mexopts\msvc90opts.bat

LFLAGS     = /MACHINE:X86 \
             /LIBPATH:"$(VCROOT)\lib" \
             /LIBPATH:"$(WINSDKROOT)\lib"

!if "$(ARCH)" == "w64"
!message === COMPILING FOR 64-BIT ===

VCROOT     = C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC
WINSDKROOT = C:\Program Files\Microsoft SDKs\Windows\v6.0A
MATLABROOT = C:\Program Files\MATLAB\R2009b

CC         = "$(VCROOT)\bin\amd64\cl.exe"
LINK       = "$(VCROOT)\bin\amd64\link.exe"
MSVCRLOC   = $(VCROOT)\redist\amd64\Microsoft.VC90.CRT
MSVCR      = msvcr90.dll
MSVCP      = msvcp90.dll
MSVCM      = msvcm90.dll
MSMANIFEST = Microsoft.VC90.CRT.manifest

MEX        = "$(MATLABROOT)\bin\mex.bat"
MEXEXT     = mexw64
MEXOPT     = $(MATLABROOT)\bin\win64\mexopts\msvc90opts.bat

LFLAGS     = /MACHINE:X64 \
	     /LIBPATH:"$(VCROOT)\lib\amd64" \
	     /LIBPATH:"$(WINSDKROOT)\lib\x64"
!endif

GIT        = git
BRANCH     = v$(VER)-$(ARCH)

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
#   /Z7                : Embedded CodeView debug info in .obj
#   /MD                : Multi-thread run-time library dynamically linked
#   /TC                : Source code is C (not C++)
#   /W3                : Usa all warnings
#   /Zp8               : Align structures to 8 bytes
#   /Ox                : Turn on optimizations
#
# LFLAGS
#   /NOLOGO            : LINK does not display splash
#   /INCREMENTAL:NO    : No incremental linking
#   /MANIFEST          : See DLL HELL below
#   /DEBUG             : Generate debug info (.pdb files)
#
# MEX_FLAGS
#   -I                 : Include VLFeat
#   -L                 : Add a library search path
#   -l                 : Link a dll
#
# ======================= ABOUT THE DLL HELL =========================
#
# This makefile compiles VLFeat to make use of the side-by-side
# deployment model. In other words, the C runtime library is
# re-distributed with the application and the appropriate manifest
# file is embedded in the binaries.
#
# References:
#   http://www.codeguru.com/forum/showthread.php?t=408061
#

bindir     = bin\$(ARCH)
mexdir     = toolbox\mex\$(MEXEXT)
objdir     = $(bindir)\objs

CFLAGS     = /nologo /TC /MD \
             /D"_CRT_SECURE_NO_DEPRECATE" \
             /D"__LITTLE_ENDIAN__" \
             /I. \
             /W1 /Z7 /Zp8 /Ox

LFLAGS     = $(LFLAGS) /NOLOGO \
             /INCREMENTAL:NO \
             /MANIFEST \
             /DEBUG

DLL_CFLAGS = /D"VL_BUILD_DLL"
EXE_LFLAGS = $(LFLAGS) /LIBPATH:"$(bindir)" vl.lib
MEX_FLAGS  = -v -f "$(MEXOPT)" -I. -Itoolbox -L"$(bindir)" -lvl

libsrc = \
  vl\aib.c \
  vl\dsift.c \
  vl\generic.c \
  vl\getopt_long.c \
  vl\hikmeans.c \
  vl\homkermap.c \
  vl\host.c \
  vl\ikmeans.c \
  vl\imopv.c \
  vl\imopv_sse2.c \
  vl\kdtree.c \
  vl\kmeans.c \
  vl\mathop.c \
  vl\mathop_sse2.c \
  vl\mser.c \
  vl\pegasos.c \
  vl\pgm.c \
  vl\quickshift.c \
  vl\random.c \
  vl\rodrigues.c \
  vl\sift.c \
  vl\stringop.c

cmdsrc = \
  src\aib.c \
  src\mser.c \
  src\sift.c \
  src\test_getopt_long.c \
  src\test_heap-def.c \
  src\test_host.c \
  src\test_imopv.c \
  src\test_mathop.c \
  src\test_mathop_abs.c \
  src\test_nan.c \
  src\test_qsort-def.c \
  src\test_rand.c \
  src\test_stringop.c \
  src\test_threads.c \
  src\test_vec_comp.c

mexsrc = \
  toolbox\aib\vl_aib.c \
  toolbox\aib\vl_aibhist.c \
  toolbox\geometry\vl_irodr.c \
  toolbox\geometry\vl_rodr.c \
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
  toolbox\misc\vl_getpid.c \
  toolbox\misc\vl_homkermap.c \
  toolbox\misc\vl_ihashfind.c \
  toolbox\misc\vl_ihashsum.c \
  toolbox\misc\vl_inthist.c \
  toolbox\misc\vl_kdtreebuild.c \
  toolbox\misc\vl_kdtreequery.c \
  toolbox\misc\vl_localmax.c \
  toolbox\misc\vl_pegasos.c \
  toolbox\misc\vl_samplinthist.c \
  toolbox\misc\vl_simdctrl.c \
  toolbox\misc\vl_twister.c \
  toolbox\misc\vl_version.c \
  toolbox\mser\vl_erfill.c \
  toolbox\mser\vl_mser.c \
  toolbox\quickshift\vl_quickshift.c \
  toolbox\sift\vl_dsift.c \
  toolbox\sift\vl_sift.c \
  toolbox\sift\vl_siftdescriptor.c \
  toolbox\sift\vl_ubcmatch.c

# horrible Make program, horrible code:
libobj = $(libsrc:vl\=bin\w32\objs\)
cmdexe = $(cmdsrc:src\=bin\w32\)
mexdll = $(mexsrc:.c=.mexw32)
mexdll = $(mexdll:toolbox\sift=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\mser=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\imop=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\geometry=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\misc=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\aib=toolbox\mex\mexw32)
mexdll = $(mexdll:toolbox\quickshift=toolbox\mex\mexw32)
mexpdb = $(mexdll:.dll=.pdb)

!if "$(ARCH)" == "w64"
libobj = $(libsrc:vl\=bin\w64\objs\)
cmdexe = $(cmdsrc:src\=bin\w64\)
mexdll = $(mexsrc:.c=.mexw64)
mexdll = $(mexdll:toolbox\sift=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\mser=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\imop=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\geometry=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\misc=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\aib=toolbox\mex\mexw64)
mexdll = $(mexdll:toolbox\quickshift=toolbox\mex\mexw64)
mexpdb = $(mexdll:.mexw64=.pdb)
!endif

libobj = $(libobj:.c=.obj)
cmdexe = $(cmdexe:.c=.exe)
cmdpdb = $(cmdexe:.exe=.pdb)

!IFDEF MATLABROOT
all: $(bindir) $(objdir) $(mexdir) $(bindir)\vl.lib $(bindir)\vl.dll $(mexdir)\vl.dll $(cmdexe) $(mexdll) $(mexdir)\$(MSMANIFEST) $(mexdir)\$(MSVCR) $(mexdir)\$(MSVCP) $(mexdir)\$(MSVCM) $(bindir)\$(MSMANIFEST) $(bindir)\$(MSVCR) $(bindir)\$(MSVCP) $(bindir)\$(MSVCM)
!ELSE
all: $(bindir) $(objdir) $(bindir)\vl.lib $(bindir)\vl.dll $(cmdexe) $(bindir)\$(MSMANIFEST) $(bindir)\$(MSVCR) $(bindir)\$(MSVCP) $(bindir)\$(MSVCM)
!ENDIF

BUILD_MEX=@echo .... CC [MEX] $(@R).dll && \
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
	if exist toolbox\mex$(ARCH) rmdir /S /Q toolbox\mex$(ARCH)

distclean:
	if exist bin\w32 rmdir /S /Q bin\w32
	if exist bin\w64 rmdir /S /Q bin\w64
	if exist toolbox\mexw32 rmdir /S /Q toolbox\mexw32
	if exist toolbox\mexw64 rmdir /S /Q toolbox\mexw64

info:
	@echo $(mexx)
	@echo ** bindir     = $(bindir)
	@echo ** mexdir     = $(mexdir)
	@echo ** objdir     = $(objdir)
	@echo ** libsrc     = $(libsrc)
	@echo ** libobj     = $(libobj)
	@echo ** cmdsrc     = $(cmdsrc)
	@echo ** cmdexe     = $(cmdexe)
	@echo ** mexsrc     = $(mexsrc)
	@echo ** mexdll     = $(mexdll)
	@echo ** CC         = $(CC)
	@echo ** CFLAGS     = $(CFLAGS)
	@echo ** DLL_CFLAGS = $(DLL_CFLAGS)
	@echo ** MEX_CFLAGS = $(MEX_CFLAGS)
	@echo ** BUILD_MEX  = "$(BUILD_MEX)"
	@echo ** MATLABROOT = $(MATLABROOT)
	@echo ** MEX_LFLAGS = $(MEX_LFLAGS)
	@echo ** MEX        = $(MEX)

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
#                                          Rules to compile VLFeat DLL
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

# --------------------------------------------------------------------
#                                                 Rules to compile EXE
# --------------------------------------------------------------------

# src\*.c -> $bindir\*.exe
{src}.c{$(bindir)}.exe:
	@echo .... CC [EXE] $(@)
	@$(CC) $(CFLAGS) /Fe"$(@)" /Fo"$(@R).obj" "$(<)" /link $(EXE_LFLAGS)
	@MT /nologo /outputresource:"$(@);#1" /manifest "$(@).manifest"
	@-del "$(@).manifest"
	@-del "$(@R).obj"

# --------------------------------------------------------------------
#                                                 Rules to compile MEX
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

# vl.dll => mexw{32,64}/vl.dll
$(mexdir)\vl.dll : $(bindir)\vl.dll
	copy "$(**)" "$(@)"

# --------------------------------------------------------------------
#                              Rules to copy redistributable libraries
# --------------------------------------------------------------------

# msvcr__.dll => bin/win{32,64}/msvcr__.dll
$(bindir)\$(MSMANIFEST): "$(MSVCRLOC)\$(MSMANIFEST)"
	copy $(**) "$(@)"

$(bindir)\$(MSVCR): "$(MSVCRLOC)\$(MSVCR)"
	copy $(**) "$(@)"

$(bindir)\$(MSVCP): "$(MSVCRLOC)\$(MSVCP)"
	copy $(**) "$(@)"

$(bindir)\$(MSVCM): "$(MSVCRLOC)\$(MSVCM)"
	copy $(**) "$(@)"

# msvcr__.dll => toolbox/mexw32/msvcr__.dll
$(mexdir)\$(MSMANIFEST): "$(MSVCRLOC)\$(MSMANIFEST)"
	copy $(**) "$(@)"

$(mexdir)\$(MSVCR): "$(MSVCRLOC)\$(MSVCR)"
	copy $(**) "$(@)"

$(mexdir)\$(MSVCP): "$(MSVCRLOC)\$(MSVCP)"
	copy $(**) "$(@)"

$(mexdir)\$(MSVCM): "$(MSVCRLOC)\$(MSVCM)"
	copy $(**) "$(@)"

# --------------------------------------------------------------------
#                                       Rules to post the binary files
# --------------------------------------------------------------------

bin-release:
	echo Fetching remote tags && \
	git fetch --tags && \
	echo Checking out v$(VER) && \
	$(GIT) checkout v$(VER)
	echo Rebuilding binaries for release
	if exist "bin\$(ARCH)" del /f /Q "bin\$(ARCH)"
	if exist "bin\mex\$(ARCH)" del /f /Q "toolbox\mex$(ARCH)"
	nmake /f Makefile.mak ARCH=$(ARCH)

bin-commit: bin-release
	@echo Fetching remote tags && \
	git fetch --tags
	@echo Crearing/resetting and checking out branch $(BRANCH) to v$(VER) && \
	$(GIT) branch -f $(BRANCH) v$(VER) && \
	$(GIT) checkout $(BRANCH)
	@echo Adding binaries && \
	$(GIT) add -f $(bindir)\vl.lib && \
	$(GIT) add -f $(bindir)\vl.dll && \
	$(GIT) add -f $(cmdexe) && \
	$(GIT) add -f $(bindir)\$(MSANIFEST)  && \
	$(GIT) add -f $(bindir)\$(MSVCP) && \
	$(GIT) add -f $(bindir)\$(MSVCR) && \
	$(GIT) add -f $(bindir)\$(MSVCM)
	@echo Adding MEX files && \
	$(GIT) add -f $(mexdll) && \
	$(GIT) add -f $(mexdir)\$(MSANIFEST)  && \
	$(GIT) add -f $(mexdir)\$(MSVCP)  && \
	$(GIT) add -f $(mexdir)\$(MSVCR)  && \
	$(GIT) add -f $(mexdir)\$(MSVCM)  && \
	$(GIT) add -f $(mexdir)\$(MSVCP)  && \
	$(GIT) add -f $(mexdir)\$(MSVCR)
	@echo Commiting changes && \
	$(GIT) commit -m "$(ARCH) binaries for version $(VER)"
	@echo Commiting and pushing to server the binaries && \
	$(GIT) push -v --force bin $(BRANCH):refs/heads/$(BRANCH) && \
	$(GIT) checkout v$(VER) && \
	$(GIT) branch -D $(BRANCH)
