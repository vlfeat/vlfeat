# file:       Makefile.mak
# author:     Andrea Vedaldi
# descrption: Microsoft NMake makefile

# --------------------------------------------------------------------
#                                                        Customization
# --------------------------------------------------------------------
#
# - MSVCR    : the file name of msvcr__.dll for your compiler
# - MSVCRLOC : must point to the location of msvcr__.dll for your compiler
# - MATLABROOT : must point to MATLAB root directory (undef = no MEX support)
# - MATLABLIB : Location of the externel libs, such as libmex and libmx.

MSVCR      = msvcr90.dll
MSVCRLOC   = C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT
MATLABROOT = C:\Program Files\MATLAB\R2008a
MATLABLIB  = "$(MATLABROOT)\extern\lib\win32\microsoft"

MSVCR      = msvcr80.dll
MSVCRLOC   = C:\Program Files\Microsoft Visual Studio 8\VC\redist\x86\Microsoft.VC80.CRT
MATLABROOT = C:\Program Files\MATLAB08a
MATLABLIB  = "$(MATLABROOT)\extern\lib\win32\microsoft"

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
#   /arch:SSE2         : Enable SSE2 instructions
#
# LFLAGS
#   /NOLOGO            : LINK does not display splash
#   /INCREMENTAL:NO    : No incremental linking
#   /MANIFEST:NO       : No manifest
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
# depoyment model. In other words, the C runtime library is
# re-destributed with the application and the appropriate manifest
# file is embedded in the binaries.
#
# References:
#   http://www.codeguru.com/forum/showthread.php?t=408061
#

bindir     = bin\win32
mexdir     = toolbox\mexw32
objdir     = $(bindir)\objs

CFLAGS     = /nologo /TC /MD \
             /D"__VISUALC__" /D"WIN32" \
             /D"__LITTLE_ENDIAN__" \
						 /D"VL_SUPPORT_SSE2" \
             /D"_CRT_SECURE_NO_DEPRECATE" \
             /I. \
             /W1 /Z7 /Zp8 /O2

DLL_CFLAGS = /D"VL_BUILD_DLL"

LFLAGS     = /NOLOGO /INCREMENTAL:NO /MANIFEST \
             /LIBPATH:$(bindir) vl.lib \
             /DEBUG

MEX_RC     = "$(MATLABROOT)\extern\include\mexversion.rc"

MEX_CFLAGS = $(CFLAGS) /I"$(MATLABROOT)\extern\include" \
             /Itoolbox \
             /DMATLAB_MEX_FILE /D_WINDLL

MEX_LFLAGS = $(LFLAGS) \
             /DLL /EXPORT:mexFunction \
             /MACHINE:X86 \
             /LIBPATH:$(MATLABLIB) \
             libmx.lib libmex.lib libmat.lib

libsrc =                \
 vl\aib.c               \
 vl\dhog.c              \
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
 vl\stringop.c
 
cmdsrc =                \
 src\sift.c             \
 src\mser.c             \
 src\aib.c              \
 src\test_getopt_long.c \
 src\test_stringop.c    \
 src\test_nan.c         \
 src\test_rand.c        \
 src\test_imopv.c       \
 src\test_host.c

mexsrc =                       \
 toolbox\sift\sift.c           \
 toolbox\sift\ubcmatch.c       \
 \
 toolbox\mser\mser.c           \
 toolbox\mser\erfill.c         \
 \
 toolbox\imop\imsmooth.c       \
 toolbox\imop\imintegral.c     \
 toolbox\imop\imwbackwardmx.c  \
 toolbox\imop\tpsumx.c         \
 \
 toolbox\geometry\rodr.c       \
 toolbox\geometry\irodr.c      \
 \
 toolbox\kmeans\ikmeans.c      \
 toolbox\kmeans\ikmeanspush.c  \
 toolbox\kmeans\hikmeans.c     \
 toolbox\kmeans\hikmeanspush.c \
 \
 toolbox\misc\alldist2.c       \
 toolbox\misc\binsum.c         \
 toolbox\misc\ihashfind.c      \
 toolbox\misc\ihashsum.c       \
 toolbox\misc\localmax.c       \
 toolbox\misc\whistc.c         \
 \
 toolbox\aib\aib.c             \
 toolbox\aib\aibhist.c

libobj = $(libsrc:vl\=bin\win32\objs\)
libobj = $(libobj:.c=.obj)
cmdexe = $(cmdsrc:src\=bin\win32\)
cmdexe = $(cmdexe:.c=.exe)
cmdpdb = $(cmdexe:.exe=.pdb)

mexdll = $(mexsrc:.c=.dll) 
mexdll = $(mexdll:toolbox\sift=toolbox\mexw32)
mexdll = $(mexdll:toolbox\mser=toolbox\mexw32)
mexdll = $(mexdll:toolbox\imop=toolbox\mexw32)
mexdll = $(mexdll:toolbox\geometry=toolbox\mexw32)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mexw32)
mexdll = $(mexdll:toolbox\misc=toolbox\mexw32)
mexdll = $(mexdll:toolbox\aib=toolbox\mexw32)
mexres = $(mexdll:.dll=.res)
mexpdb = $(mexdll:.dll=.pdb)

!IFDEF MATLABROOT
all: $(objdir) $(mexdir) $(bindir)\vl.lib $(bindir)\vl.dll $(mexdir)\vl.dll $(cmdexe) $(mexdll) $(mexdir)\$(MSVCR) $(bindir)\$(MSVCR)
!ELSE
all: $(objdir) $(bindir)\vl.lib $(bindir)\vl.dll $(cmdexe) $(mexdir)\$(MSVCR) $(bindir)\$(MSVCR)
!ENDIF

BUILD_DLL=@echo .... CC [MEX] $(@R).dll && \
	$(CC) $(MEX_CFLAGS) /c /Fo"$(@R).obj" "$(<)" && \
	RC /fo"$(@R).res" $(MEX_RC) && \
	LINK $(MEX_LFLAGS) "$(@R).res" "$(@R).obj" /OUT:$(@) && \
	MT /nologo /outputresource:"$(@)";^#2 /manifest "$(@).manifest" && \
	del "$(@R).obj" "$(@R).exp" "$(@R).lib" "$(@R).res" "$(@).manifest"

# --------------------------------------------------------------------
#                                                    Maintenance rules
# --------------------------------------------------------------------

clean:
	del /f /Q $(libobj)
	del /f /Q $(objdir)
	del /f /Q $(cmdpdb)
	del /f /Q $(mexpdb)
	del /f /Q $(mexdir)\$(MSVCR)
	del /f /Q $(mexdir)\vl.dll
	del /f /Q bin\win32\$(MSVCR)
	
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
	@echo ** BUILD_DLL  = $(BUILD_DLL)
	@echo ** MEX_LFLAGS = $(MEX_LFLAGS)

# --------------------------------------------------------------------
#                                                          Build rules
# --------------------------------------------------------------------

# create directory if missing
$(objdir) :
	mkdir $(objdir)

$(mexdir) :
	mkdir $(mexdir)

# special sources with SSE2 support
$(objdir)\imopv_sse2.obj : vl\imopv_sse2.c
	@echo .... CC [+SSE2] $(@)
	@$(CC) $(CFLAGS) $(DLL_CFLAGS) /arch:SSE2 /D"__SSE2__" /c /Fo"$(@)" "vl\$(@B).c"
	
# vl\*.c -> $objdir\*.obj
{vl}.c{$(objdir)}.obj:
	@echo .... CC $(@)
	@$(CC) $(CFLAGS) $(DLL_CFLAGS) /c /Fo"$(@)" "$(<)"
	
# src\*.c -> $bindir\*.exe
{src}.c{$(bindir)}.exe:
	@echo .... CC [EXE] $(@)
	@$(CC) $(CFLAGS) /Fe"$(@)" /Fo"$(@R).obj" "$(<)" /link $(LFLAGS)
	@MT /nologo /outputresource:"$(@);#1" /manifest "$(@).manifest"
	@-del "$(@).manifest"
	@-del "$(@R).obj"

# toolbox\*.c -> toolbox\*.dll
{toolbox\sift}.c{$(mexdir)}.dll:
	$(BUILD_DLL)

{toolbox\mser}.c{$(mexdir)}.dll:
	$(BUILD_DLL)

{toolbox\imop}.c{$(mexdir)}.dll:
	$(BUILD_DLL)

{toolbox\geometry}.c{$(mexdir)}.dll:
	$(BUILD_DLL)

{toolbox\kmeans}.c{$(mexdir)}.dll:
	$(BUILD_DLL)

{toolbox\aib}.c{$(mexdir)}.dll:
	$(BUILD_DLL)

{toolbox\misc}.c{$(mexdir)}.dll:
	$(BUILD_DLL)

# Link VLFeat DLL
$(bindir)\vl.dll : $(libobj)
	@echo .. LINK [DLL] $(@R).dll
	@link /DLL $(**) /OUT:"$(@)" /NOLOGO /MANIFEST
	@mt /nologo /outputresource:"$(@);#2" /manifest "$(@R).dll.manifest"
	@-del "$(@R).dll.manifest"

# *.obj -> *.lib
$(bindir)\vl.lib : $(libobj)
	@echo ... LIB $(@R).lib
	@lib $(**) /OUT:"$(@)" /NOLOGO

# vl.dll => mexw32/vl.dll
$(mexdir)\vl.dll : $(bindir)\vl.dll
	copy "$(**)" "$(@)"

# msvcr__.dll => bin/win32/msvcr__.dll
$(bindir)\$(MSVCR): "$(MSVCRLOC)\$(MSVCR)"
	copy $(**) "$(@)"

# msvcr__.dll => toolbox/mexw32/msvcr__.dll
$(mexdir)\$(MSVCR): "$(MSVCRLOC)\$(MSVCR)"
	copy $(**) "$(@)"
