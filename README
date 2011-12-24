                     VisionLab Features Library
                  Andrea Vedaldi and Brian Fulkerson

ABOUT
  The VLFeat open source library implements popular computer vision
  algorithms including SIFT, MSER, k-means, hierarchical k-means,
  agglomerative information bottleneck, and quick shift. It is written
  in C for efficiency and compatibility, with interfaces in MATLAB for
  ease of use, and detailed documentation throughout. It supports
  Windows, Mac OS X, and Linux.

  VLFeat is distributed under the BSD license (see the COPYING file).

  Most of the documentation is available online at
  http://www.vlfeat.org/index.html or compiled in HTML format in
  `doc/index.html'.

INSTALLING
  If you downloaded VLFeat binary package, installation is trivial.
  The binaries are found in the `bin/ARCH' directory, where ARCH
  depends on the architecture of your machine. VLFeat supports the
  following ones out of the box:

    ARCH     MEXEXT     DESCRIPTION
    -----------------------------------------------
    maci     mexmaci    Mac OS X Intel
    maci64   mexmaci64  Mac OS X Intel (64 bit)
    glnx86   mexglx     GNU Linux
    glnxa64  mexa64     GNU Linux (64 bit)
    win32    mexw32     Windows
    win64    mexw64     Windows (64 bit)

  The `bin' directory contains also a copy of the VLFeat DLL (and of
  the appropriate C runtime DLL in the case of Windows).

  The VLFeat MATLAB toolbox can be found in the `toolbox'
  directory. To use it with MATLAB, run the VL_SETUP command as

  > cd VLFEATROOT/toolbox ; vl_setup

  or

  > run VLFEATROOT/toolbox/vl_setup

  You can make this permanent by adding the line to MATLAB startup.m
  file.

  An HTML copy of the documentation can be found in `doc/index.html'. The
  same documentation is available online at http://www.vlfeat.org/index.html.

COMPILING
  If you downloaded VLFeat source package or just want to compile your
  own version, all you need is a standard compiler setup.

  UNIX. In Mac OS X and Linux you will need GNU GCC and GNU Make. In
  fact, most C-90 compilers will do, although a few C-99 features are
  used.

  To compile MATLAB support, MATLAB should also be installed and the
  MATLAB MEX command should be available from the command line.

  In general, issuing

  > make

  should be enough to compile VLFeat. Type

  > make help

  or refer to http://www.vlfeat.org for further instructions.

  WINDOWS. In Windows you will need Visual Studio 2008 or 2010.
  VLFeat includes a Microsoft NMAKE Makefile (Makefile.mak). Open the
  Visual C command prompty (make sure you run the 64 bit version if
  your host is 64 bit), into the VLFeat directory and issue.

  > nmake /f Makefile.mak ARCH=win32

  or

  > nmake /f Makefile.mak ARCH=win64

  depending on your architecture.


CREATING THE DOCUMENTATION
  Creating the documentation requires a UNIX platform (either Linux
  or Mac OS X) and the following tools

  - fig2dev (part of transfig)
  - a modern LaTeX with
    + pdflatex
    + dvips
    + htlatex (possibily part of TeX4ht)
    + dvipng  (possibily a separated package)
  - doxygen (a recent version)

  FIGURES. Figures are preprocessed by typing

  > make -C doc/figures

  However, you need to run MATLAB programs to generate most of the
  figures to start with. To this end, load MATLAB and (provided that
  everything is compiled and installed correctly) type

  > vl_demo

  TUTORIALS. To create the figures for the tutorials, issue

  > make doc-deep
  > make doc

  SOURCE CODE DOCUMENTATION. To compile the source code documentation
  issue

  > make doc-api

  The start of the documentation is the file `doc/index.html'.

APPENDIX
  CODE COMPATIBILITY. In addition to the C-90 standard, the C compiler
  is supposed to support the following common features:

  - long int (64 bit integer) support
  - variadic macro support

  The SSE accelerated code requires the compiler to support Intel
  intrisic. GCC and Visual C satisfy all the requirements.

CHANGES
  0.9.14     Added SLIC superpixels and VL_ALPHANUM(). Improved
             Windows binary package and added support for Visual
             Studio 2010. Improved the documentation layout and added
             a proper bibliography. Bugfixes and other minor
             improvements. Moved from the GPL to the less restrictive
             BSD license.
  0.9.13     Fixes Windows binary package.
  0.9.12     Fixes vl_compile and the architecture string on Linux 32 bit.
  0.9.11     Fixes a compatibility problem on older Mac OS X versions.
             A few bugfixes are included too.
  0.9.10     Improves the homogeneous kernel map. Plenty of small tweaks
             and improvements. Make maci64 the default architecture on
             the Mac.
  0.9.9      Added: sift matching example. Extended Caltech-101
             classification example to use kd-trees.
  0.9.8      Added: image distance transform, PEGASOS, floating point
             K-means, homogeneous kernel maps, a Caltech-101
             classification example. Improved documentation.
  0.9.7      Changed the Mac OS X binary distribution to require
             a less recent version of Mac OS X (10.5).
  0.9.6      Changed the GNU/Linux binary distribution to require
             a less recent version of the C library.
  0.9.5      Added kd-tree and new SSE-accelerated vector/histogram
             comparison code.  Improved dense SIFT (dsift) implementation.
             Added Snow Leopard and MATLAB R2009b support.
  0.9.4      Added quick shift. Renamed dhog to dsift and improved
             implementation and documentation. Improved tutorials.
             Added 64 bit Windows binaries. Many other small changes.
  0.9.3      Namespace change (everything begins with a vl_ prefix
             now). Many other changes to provide compilation support
             on Windows with MATLAB 7.
  beta-3     Completions to the ikmeans code.
  beta-2     Many completions.
  beta-1     Initial public release.
