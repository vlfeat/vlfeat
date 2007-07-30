# file:       Makefile.mak
# author:     Andrea Vedaldi
# descrption: Microsoft NMake makefile

MATLABROOT = C:\MATLAB7
bindir     = bin\win32
objdir     = bin\win32\objs
CFLAGS     = /D"__VISUALC__" /D"WIN32" \
             /D"__LITTLE_ENDIAN__" \
             /D"_CRT_SECURE_NO_DEPRECATE" \
             /I. /W3 /nologo /Z7 /TC /MT
LFLAGS     = /NOLOGO /INCREMENTAL:NO /MANIFEST:NO \
             /LIBPATH:$(bindir) vl.lib /SUBSYSTEM:CONSOLE
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
 vl\mser.c

cmdsrc =                \
 src/sift.c             \
 src/mser.c             \
 src/test_getopt_long.c \
 src/test_stringop.c

mexsrc =                \
 toolbox\sift.c         \
 toolbox\mser.c         \
 toolbox\erfill.c

libobj = $(libsrc:vl\=bin\win32\objs\)
libobj = $(libobj:.c=.obj)
cmdexe = $(cmdsrc:src/=bin\win32\)
cmdexe = $(cmdexe:.c=.exe)
mexdll = $(mexsrc:.c=.dll)
mexres = $(mexsrc:.c=.res)

all: $(objdir) $(bindir)\vl.lib $(cmdexe) $(mexdll)

# --------------------------------------------------------------------
#                                                    Maintenance rules
# --------------------------------------------------------------------

clean:
	-del $(libobj)
	-del /Q $(objdir)

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
	$(CC) $(CFLAGS) /c /Fo"$(@)" "$(<)"

# src\*.c -> $bindir\*.exe
{src}.c{$(bindir)}.exe:
	$(CC) $(CFLAGS) /Fe"$(@)" /Fo"$(@R).obj" "$(<)" /link $(LFLAGS)
	-del "$(@R).obj"

# toolbox\*.c -> tooblox\*.dll
{toolbox}.c{toolbox}.dll:
	$(CC) $(MEX_CFLAGS) /c /Fo"$(@R).obj" "$(<)"
	RC /fo"$(@R).res" $(MEX_RC)
	LINK $(MEX_LFLAGS) "$(@R).res" "$(@R).obj" /OUT:$(@)
	-del "$(@R).obj"
	-del "$(@R).exp"
	-del "$(@R).lib"
	-del "$(@R).res"

# *.obj -> *.lib
$(bindir)\vl.lib : $(libobj)
	lib $(**) /OUT:"$(@)" /NOLOGO
