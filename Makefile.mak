# file:       Makefile.mak
# author:     Andrea Vedaldi
# author:     Brian Fulkerson
# descrption: Microsoft NMake makefile

# --------------------------------------------------------------------
#                                                        Customization
# --------------------------------------------------------------------
#
# - MSVCR    : the file name of msvcr__.dll for your compiler
# - MSVCRLOC : must point to the location of msvcr__.dll for your compiler
# - MATLABROOT : must point to MATLAB root directory (undef = no MEX support)
# - MATLABLIB : Location of the external libs, such as libmex and libmx.

ARCH = win32

VCROOT     = C:\Program Files\Microsoft Visual Studio 9.0\VC
WINSDKROOT = C:\Program Files\Microsoft SDKs\Windows\v6.0A
MATLABROOT = C:\Program Files\MATLAB\R2008b

CC         = "$(VCROOT)\bin\cl.exe"
LINK       = "$(VCROOT)\bin\link.exe"
MSVCRLOC   = $(VCROOT)\redist\x86\Microsoft.VC90.CRT
MSVCR      = msvcr90.dll 
MSVCP      = msvcp90.dll 
MSVCM      = msvcm90.dll
MSMANIFEST = Microsoft.VC90.CRT.manifest
MATLABLIB  = $(MATLABROOT)\extern\lib\win32\microsoft
MEX_SFX    = mexw32

LFLAGS     = /MACHINE:X86 \
	     /LIBPATH:"$(VCROOT)\lib" \
	     /LIBPATH:"$(WINSDKROOT)\lib"

!if "$(ARCH)" == "win64"
!message === COMPILING FOR 64-BIT ===

VCROOT     = C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC
WINSDKROOT = C:\Program Files\Microsoft SDKs\Windows\v6.0A
MATLABROOT = C:\Program Files\MATLAB\R2008b

CC         = "$(VCROOT)\bin\amd64\cl.exe"
LINK       = "$(VCROOT)\bin\amd64\link.exe"
MSVCRLOC   = $(VCROOT)\redist\amd64\Microsoft.VC90.CRT
MSVCR      = msvcr90.dll 
MSVCP      = msvcp90.dll 
MSVCM      = msvcm90.dll
MSMANIFEST = Microsoft.VC90.CRT.manifest
MATLABLIB  = $(MATLABROOT)\extern\lib\win64\microsoft
MEX_SFX    = mexw64

LFLAGS     = /MACHINE:X64 \
	     /LIBPATH:"$(VCROOT)\lib\amd64" \
	     /LIBPATH:"$(WINSDKROOT)\lib\x64"
!endif

# Here is an example of how the variables might look with a different version
# of Visual Studio and an alternate location for Matlab
#MSVCR      = msvcr80.dll 
#MSVCP      = msvcp80.dll 
#MSVCM      = msvcm80.dll
#MSMANIFEST = Microsoft.VC80.CRT.manifest
#MSVCRLOC   = C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT
#MATLABROOT = C:\Program Files\MATLAB08a
#MATLABLIB  = $(MATLABROOT)\extern\lib\$(ARCH)\microsoft

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
#   /MT                : Multi-thread run-time library
#   /TC                : Source code is C (not C++)
#   /W3                : Usa all warnings
#   /Zp8               : Align structures to 8 bytes
#   /O2                : Optimize for speed
#
# LFLAGS
#   /NOLOGO            : LINK does not display splash
#   /INCREMENTAL:NO    : No incremental linking
#   /MANIFEST          : See DLL HELL below
#   /DEBUG             : Generate debug info (.pdb files)
#
# MEX_RC               : MEX .rc file location
#
# MEX_CFLAGS
#   /D_WINDLL          : Signal DLL code
#   /DMATLAB_MEX_FILE  : Signal MATLAB MEX code
#   /Itoolbox          : Add toolbox to inc
#
# MEX_LFLAGS
#  /DLL                : Produce a DLL
#  /EXPORT:mexFunction : Export MEX file entry point
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
mexdir     = toolbox\$(MEX_SFX)
objdir     = $(bindir)\objs

CFLAGS     = /nologo /TC /MD \
             /D"_CRT_SECURE_NO_DEPRECATE" \
             /D"__LITTLE_ENDIAN__" \
             /D"VL_SUPPORT_SSE2" \
             /I. \
             /W1 /Z7 /Zp8 /O2

DLL_CFLAGS = /D"VL_BUILD_DLL"

LFLAGS     = $(LFLAGS) /NOLOGO \
	     /INCREMENTAL:NO \
	     /MANIFEST \
             /DEBUG

EXE_LFLAGS = $(LFLAGS) \
	     /LIBPATH:"$(bindir)" vl.lib 

MEX_RC     = "$(MATLABROOT)\extern\include\mexversion.rc"

MEX_CFLAGS = $(CFLAGS) \
             /I"$(MATLABROOT)\extern\include" \
             /Itoolbox \
             /DMATLAB_MEX_FILE /D_WINDLL

MEX_LFLAGS = $(LFLAGS) \
             /DLL \
             /EXPORT:mexFunction \
             /LIBPATH:"$(bindir)" vl.lib \
             /LIBPATH:"$(MATLABLIB)" libmx.lib libmex.lib libmat.lib

libsrc =                \
 vl\aib.c               \
 vl\dsift.c             \
 vl\generic.c           \
 vl\getopt_long.c       \
 vl\hikmeans.c          \
 vl\host.c              \
 vl\ikmeans.c           \
 vl\imop.c              \
 vl\imopv.c             \
 vl\imopv_sse2.c        \
 vl\mathop.c            \
 vl\mser.c              \
 vl\pgm.c               \
 vl\random.c            \
 vl\rodrigues.c         \
 vl\sift.c              \
 vl\quickshift.c        \
 vl\stringop.c
 
cmdsrc =                \
 src\aib.c              \
 src\mser.c             \
 src\sift.c             \
 src\test_getopt_long.c \
 src\test_host.c        \
 src\test_imopv.c       \
 src\test_mathop.c      \
 src\test_nan.c         \
 src\test_rand.c        \
 src\test_stringop.c

mexsrc =                          \
 toolbox\aib\vl_aib.c             \
 toolbox\aib\vl_aibhist.c         \
 toolbox\geometry\vl_irodr.c      \
 toolbox\geometry\vl_rodr.c       \
 toolbox\imop\vl_imintegral.c     \
 toolbox\imop\vl_imsmooth.c       \
 toolbox\imop\vl_imwbackwardmx.c  \
 toolbox\imop\vl_tpsumx.c         \
 toolbox\kmeans\vl_hikmeans.c     \
 toolbox\kmeans\vl_hikmeanspush.c \
 toolbox\kmeans\vl_ikmeans.c      \
 toolbox\kmeans\vl_ikmeanspush.c  \
 toolbox\misc\vl_alldist2.c       \
 toolbox\misc\vl_binsearch.c      \
 toolbox\misc\vl_binsum.c         \
 toolbox\misc\vl_getpid.c         \
 toolbox\misc\vl_ihashfind.c      \
 toolbox\misc\vl_ihashsum.c       \
 toolbox\misc\vl_inthist.c        \
 toolbox\misc\vl_localmax.c       \
 toolbox\misc\vl_samplinthist.c   \
 toolbox\misc\vl_twister.c        \
 toolbox\misc\vl_whistc.c         \
 toolbox\mser\vl_erfill.c         \
 toolbox\mser\vl_mser.c           \
 toolbox\quickshift\vl_quickshift.c \
 toolbox\sift\vl_dsift.c          \
 toolbox\sift\vl_sift.c           \
 toolbox\sift\vl_siftdescriptor.c \
 toolbox\sift\vl_ubcmatch.c


# horrible Make program, horrible code:
libobj = $(libsrc:vl\=bin\win32\objs\)
cmdexe = $(cmdsrc:src\=bin\win32\)
mexdll = $(mexsrc:.c=.mexw32) 
mexdll = $(mexdll:toolbox\sift=toolbox\mexw32)
mexdll = $(mexdll:toolbox\mser=toolbox\mexw32)
mexdll = $(mexdll:toolbox\imop=toolbox\mexw32)
mexdll = $(mexdll:toolbox\geometry=toolbox\mexw32)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mexw32)
mexdll = $(mexdll:toolbox\misc=toolbox\mexw32)
mexdll = $(mexdll:toolbox\aib=toolbox\mexw32)
mexdll = $(mexdll:toolbox\quickshift=toolbox\mexw32)
mexres = $(mexdll:.dll=.res)
mexpdb = $(mexdll:.dll=.pdb)

!if "$(ARCH)" == "win64"
libobj = $(libsrc:vl\=bin\win64\objs\)
cmdexe = $(cmdsrc:src\=bin\win64\)
mexdll = $(mexsrc:.c=.mexw64) 
mexdll = $(mexdll:toolbox\sift=toolbox\mexw64)
mexdll = $(mexdll:toolbox\mser=toolbox\mexw64)
mexdll = $(mexdll:toolbox\imop=toolbox\mexw64)
mexdll = $(mexdll:toolbox\geometry=toolbox\mexw64)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mexw64)
mexdll = $(mexdll:toolbox\misc=toolbox\mexw64)
mexdll = $(mexdll:toolbox\aib=toolbox\mexw64)
mexdll = $(mexdll:toolbox\quickshift=toolbox\mexw64)
mexres = $(mexdll:.mexw64=.res)
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
	$(CC) $(MEX_CFLAGS) /c /Fo"$(@R).obj" "$(<)" && \
	$(RC) /fo"$(@R).res" $(MEX_RC) && \
	$(LINK) $(MEX_LFLAGS) "$(@R).res" "$(@R).obj" /OUT:$(@) && \
	mt /nologo /outputresource:"$(@)";^#2 /manifest "$(@).manifest" && \
	del "$(@R).obj" "$(@R).exp" "$(@R).lib" "$(@R).res" "$(@).manifest"

# --------------------------------------------------------------------
#                                                    Maintenance rules
# --------------------------------------------------------------------

clean:
	del /f /Q $(libobj)
	del /f /Q $(objdir)
	del /f /Q $(cmdpdb)
	del /f /Q $(mexpdb)
	del /f /Q $(mexdir)\$(MSMANIFEST)
	del /f /Q $(mexdir)\$(MSVCR)
	del /f /Q $(mexdir)\$(MSVCP)
	del /f /Q $(mexdir)\$(MSVCM)
	del /f /Q $(mexdir)\vl.dll
	del /f /Q $(bindir)\$(MSMANIFEST)
	del /f /Q $(bindir)\$(MSVCR)
	del /f /Q $(bindir)\$(MSVCP)
	del /f /Q $(bindir)\$(MSVCM)
	del /f /Q $(bindir)\vl.dll

distclean: clean
	del /f /Q $(cmdexe)
	del /f /Q $(mexdll)

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
	@$(LINK) /DLL  $(LFLAGS) $(**) /OUT:"$(@)"
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

# toolbox\*.c -> toolbox\*.dll
{toolbox\sift}.c{$(mexdir)}.$(MEX_SFX):
	$(BUILD_MEX)

{toolbox\mser}.c{$(mexdir)}.$(MEX_SFX):
	$(BUILD_MEX)

{toolbox\imop}.c{$(mexdir)}.$(MEX_SFX):
	$(BUILD_MEX)

{toolbox\geometry}.c{$(mexdir)}.$(MEX_SFX):
	$(BUILD_MEX)

{toolbox\kmeans}.c{$(mexdir)}.$(MEX_SFX):
	$(BUILD_MEX)

{toolbox\aib}.c{$(mexdir)}.$(MEX_SFX):
	$(BUILD_MEX)

{toolbox\quickshift}.c{$(mexdir)}.$(MEX_SFX):
	$(BUILD_MEX)

{toolbox\misc}.c{$(mexdir)}.$(MEX_SFX):
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

