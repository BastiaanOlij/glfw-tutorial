This folder contains compiled binaries for the GLEW 1.12.0 library.

Note: on Mac build against i386 and x86_64 architectures by editing config\Makefile.darwin and adding:
AR = LIBTOOL
STRIP =
CFLAGS.EXTRA += -arch i386 -arch x86_64
LDFLAGS.EXTRA += -arch i386 -arch x86_64
LDFLAGS.STATIC += -static -o

Also added the main makefile and replace $(AR) cr with $(AR) $(LDFLAGS.STATIC)

Then to build:
make

Original source code can be found here:
http://glew.sourceforge.net/