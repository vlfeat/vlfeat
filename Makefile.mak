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
#
# MEX_LFLAGS
#  /DLL                : Produce a DLL
#  /EXPORT:mexFunction : Export MEX file entry point

bindir     = bin\win32
objdir     = $(bindir)\objs

CFLAGS     = /nologo /TC /MT \
             /D"__VISUALC__" /D"WIN32" \
             /D"__LITTLE_ENDIAN__" \
             /D"_CRT_SECURE_NO_DEPRECATE" \
             /I. \
             /W1 /Wp64 /Z7 /Zp8 /O2 /arch:SSE2

LFLAGS     = /NOLOGO /INCREMENTAL:NO /MANIFEST:NO \
             /LIBPATH:$(bindir) vl.lib \
             /DEBUG

MEX_RC     = $(MATLABROOT)\extern\include\mexversion.rc

MEX_CFLAGS = $(CFLAGS) /I$(MATLABROOT)\extern\include \
             /DMATLAB_MEX_FILE /D_WINDLL

MEX_LFLAGS = $(LFLAGS) \
             /DLL /EXPORT:mexFunction \
             /MACHINE:X86 \
             /LIBPATH:$(MATLABROOT)\extern\lib\win32\microsoft\msvc71 \
             libmx.lib libmex.lib libmat.lib  

libsrc =            	\
 vl\generic.c           \
 vl\getopt_long.c       \
 vl\imop.c              \
 vl\mathop.c            \
 vl\stringop.c          \
 vl\pgm.c               \
 vl\sift.c              \
 vl\mser.c              \
 vl\rodrigues.c         \
 vl\aib.c               \
 vl\ikmeans.c

cmdsrc =                \
 src\sift.c             \
 src\mser.c             \
 src\aib.c              \
 src\test_getopt_long.c \
 src\test_stringop.c    \
 src\test_nan.c

mexsrc =                \
 toolbox\sift.c         \
 toolbox\mser.c         \
 toolbox\erfill.c       \
 toolbox\imsmooth.c     \
 toolbox\rodr.c         \
 toolbox\irodr.c        \
 toolbox\imwbackwardmx.c\
 toolbox\ikmeans.c      \
 toolbox\ikmeanspush.c  \
 toolbox\tpsumx.c       \
 toolbox\whistc.c       \
 toolbox\binsum.c       \
 toolbox\alldist2.c     \
 toolbox\aib.c          \
 toolbox\ubcmatch.c

libobj = $(libsrc:vl\=bin\win32\objs\)
libobj = $(libobj:.c=.obj)
cmdexe = $(cmdsrc:src\=bin\win32\)
cmdexe = $(cmdexe:.c=.exe)
cmdpdb = $(cmdexe:.exe=.pdb)
mexdll = $(mexsrc:.c=.dll)
mexres = $(mexsrc:.c=.res)
mexpdb = $(mexsrc:.c=.pdb)

!IFDEF MATLABROOT
all: $(objdir) $(bindir)\vl.lib $(cmdexe) $(mexdll)
!ELSE
all: $(objdir) $(bindir)\vl.lib $(cmdexe)
!ENDIF

# --------------------------------------------------------------------
#                                                    Maintenance rules
# --------------------------------------------------------------------

clean:
	-del $(libobj)
	-del /Q $(objdir)
	-del $(cmdpdb)
	-del $(mexpdb)

distclean: clean
	-del $(cmdexe)
	-del $(mexdll)
info:
	@echo ** bindir     = $(bindir)
	@echo ** objdir     = $(objdir)
	@echo ** libsrc     = $(libsrc)
	@echo ** libobj     = $(libobj)
	@echo ** cmdsrc     = $(cmdsrc)
	@echo ** cmdexe     = $(cmdexe)
	@echo ** mexsrc     = $(mexsrc)
	@echo ** mexdll     = $(mexdll)
	@echo ** CC         = $(CC)
	@echo ** CFLAGS     = $(CFLAGS)
	@echo ** MEX_CFLAGS = $(MEX_CFLAGS)
	@echo ** MEX_LFLAGS = $(MEX_LFLAGS)

# --------------------------------------------------------------------
#                                                          Build rules
# --------------------------------------------------------------------

# create directory if missing
$(objdir) :
	mkdir $(objdir)

# vl\*.c -> $objdir\*.obj
{vl}.c{$(objdir)}.obj:
	@echo CC  $(<) ===^> $(@)
	@$(CC) $(CFLAGS) /c /Fo"$(@)" "$(<)"

# src\*.c -> $bindir\*.exe
{src}.c{$(bindir)}.exe:
	@echo CC  $(<) ===^> $(@)
	@$(CC) $(CFLAGS) /Fe"$(@)" /Fo"$(@R).obj" "$(<)" /link $(LFLAGS)
	@-del "$(@R).obj"

# toolbox\*.c -> tooblox\*.dll
{toolbox}.c{toolbox}.dll:
	@echo CC  $(<) ===^> $(@R).dll
	@$(CC) $(MEX_CFLAGS) /c /Fo"$(@R).obj" "$(<)"
	@RC /fo"$(@R).res" $(MEX_RC)
	@LINK $(MEX_LFLAGS) "$(@R).res" "$(@R).obj" /OUT:$(@)
	@-del "$(@R).obj"
	@-del "$(@R).exp"
	@-del "$(@R).lib"
	@-del "$(@R).res"

# *.obj -> *.lib
$(bindir)\vl.lib : $(libobj)
	@echo LIB ^*.obj ===^> $(@R).dll
	@lib $(**) /OUT:"$(@)" /NOLOGO
