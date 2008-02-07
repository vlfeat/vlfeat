# file:       Makefile.mak
# author:     Andrea Vedaldi
# descrption: Microsoft NMake makefile

# Customization:
# - MATLABROOT : must point to MATLAB root directory (undef = no MEX support)

MATLABROOT = C:\MATLAB7

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

bindir     = bin\win32
mexdir     = toolbox\mexwin
objdir     = $(bindir)\objs

CFLAGS     = /nologo /TC /MD \
             /D"__VISUALC__" /D"WIN32" \
             /D"__LITTLE_ENDIAN__" \
             /D"_CRT_SECURE_NO_DEPRECATE" \
             /I. \
             /W1 /Z7 /Zp8 /O2 /arch:SSE2

DLL_CFLAGS = /D"VL_BUILD_DLL"

LFLAGS     = /NOLOGO /INCREMENTAL:NO /MANIFEST:NO \
             /LIBPATH:$(bindir) vl_dll.lib \
             /DEBUG

MEX_RC     = $(MATLABROOT)\extern\include\mexversion.rc

MEX_CFLAGS = $(CFLAGS) /I$(MATLABROOT)\extern\include \
             /Itoolbox \
             /DMATLAB_MEX_FILE /D_WINDLL

MEX_LFLAGS = $(LFLAGS) \
             /DLL /EXPORT:mexFunction \
             /MACHINE:X86 \
             /LIBPATH:$(MATLABROOT)\extern\lib\win32\microsoft\msvc71 \
             libmx.lib libmex.lib libmat.lib  

libsrc =            	\
 vl\aib.c               \
 vl\generic.c           \
 vl\getopt_long.c       \
 vl\hikmeans.c          \
 vl\ikmeans.c           \
 vl\imop.c              \
 vl\mathop.c            \
 vl\mser.c              \
 vl\pgm.c               \
 vl\random.c		    \
 vl\rodrigues.c         \
 vl\sift.c              \
 vl\stringop.c

cmdsrc =                \
 src\sift.c             \
 src\mser.c             \
 src\aib.c              \
 src\test_getopt_long.c \
 src\test_stringop.c    \
 src\test_nan.c

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
mexdll = $(mexdll:toolbox\sift=toolbox\mexwin)
mexdll = $(mexdll:toolbox\mser=toolbox\mexwin)
mexdll = $(mexdll:toolbox\imop=toolbox\mexwin)
mexdll = $(mexdll:toolbox\geometry=toolbox\mexwin)
mexdll = $(mexdll:toolbox\kmeans=toolbox\mexwin)
mexdll = $(mexdll:toolbox\misc=toolbox\mexwin)
mexdll = $(mexdll:toolbox\aib=toolbox\mexwin)
mexres = $(mexdll:.dll=.res)
mexpdb = $(mexdll:.dll=.pdb)

!IFDEF MATLABROOT
all: $(objdir) $(mexdir) $(bindir)\vl.lib $(bindir)\vl.dll $(mexdir)\vl.dll $(cmdexe) $(mexdll)
!ELSE
all: $(objdir) $(bindir)\vl.lib $(bindir)\vl.dll $(cmdexe)
!ENDIF

BUILD_DLL=@echo CC  $(<) ]===} $(@R).dll && \
	$(CC) $(MEX_CFLAGS) /c /Fo"$(@R).obj" "$(<)" && \
	RC /fo"$(@R).res" $(MEX_RC) && \
	LINK $(MEX_LFLAGS) "$(@R).res" "$(@R).obj" /OUT:$(@) && \
	del "$(@R).obj" "$(@R).exp" "$(@R).lib" "$(@R).res" 

# --------------------------------------------------------------------
#                                                    Maintenance rules
# --------------------------------------------------------------------

clean:
	-del $(libobj)
	-del /Q $(objdir)
	-del $(cmdpdb)
	-del $(mexpdb)
	-del bin\win32\vl.dll
	-del bin\win32\vl_dll.lib

distclean: clean
	-del $(cmdexe)
	-del $(mexdll)
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

# vl\*.c -> $objdir\*.obj
{vl}.c{$(objdir)}.obj:
	@echo CC  $(<) ===^> $(@)
	@$(CC) $(CFLAGS) $(DLL_CFLAGS) /c /Fo"$(@)" "$(<)"

# src\*.c -> $bindir\*.exe
{src}.c{$(bindir)}.exe:
	@echo CC  $(<) ===^> $(@)
	@$(CC) $(CFLAGS) /Fe"$(@)" /Fo"$(@R).obj" "$(<)" /link $(LFLAGS)
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

$(bindir)\vl.dll : $(libobj)
	@echo LINK ^*.obj ===^> $(@R).dll
	link /DLL $(**) /OUT:"$(@)" /IMPlIB:"$(@R)_dll.lib" /NOLOGO

# *.obj -> *.lib
$(bindir)\vl.lib : $(libobj)
	@echo LIB ^*.obj ===^> $(@R).lib
	@lib $(**) /OUT:"$(@)" /NOLOGO

# vl.lib => mexwin/vl.lib
$(mexdir)\vl.dll : $(bindir)\vl.dll
	copy "$(**)" "$(@)"

