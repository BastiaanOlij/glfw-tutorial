This folder contains compiled binaries for the GLEW 1.13.0 library.

Note: on Mac build against i386 and x86_64 architectures by editing config\Makefile.darwin and adding:
AR = LIBTOOL
STRIP =
CFLAGS.EXTRA += -arch i386 -arch x86_64
LDFLAGS.EXTRA += -arch i386 -arch x86_64
LDFLAGS.STATIC += -static -o

Also added the main makefile and replace $(AR) cr with $(AR) $(LDFLAGS.STATIC)

Then to build:
make

Alternatively, get the source for my fork of the GLEW project and compile by using:
make SYSTEM=darwin-universal

This change has been submitted to the main project *fingers crossed*

Original source code can be found here:
http://glew.sourceforge.net/