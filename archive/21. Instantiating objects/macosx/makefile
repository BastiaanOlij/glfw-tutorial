# Compiler directives for Mac OS X
CC = gcc
CPP = g++
# -g flag adds debug info, should remove in production
CFLAGS = -g -c -arch i386 -arch x86_64 -I../include -I../3rdparty
LDFLAGS = -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -arch i386 -arch x86_64

APPNAME = glfw-tutorial
OBJECTDIR = ../build/Objects
CONTENTSDIR = ../build/$(APPNAME).app/Contents
RESOURCEDIR = $(CONTENTSDIR)/Resources

OBJECTS = $(patsubst ../source/%,$(OBJECTDIR)/%,$(patsubst %.c,%.o,$(wildcard ../source/*.c)))
OBJECTS += $(patsubst ../source/%,$(OBJECTDIR)/%,$(patsubst %.cpp,%.o,$(wildcard ../source/*.cpp)))
RESOURCES = $(patsubst ../Resources/Fonts/%,$(RESOURCEDIR)/Fonts/%,$(wildcard ../Resources/Fonts/*.*))
RESOURCES += $(patsubst ../Resources/Models/%,$(RESOURCEDIR)/Models/%,$(wildcard ../Resources/Models/*.*))
RESOURCES += $(patsubst ../Resources/Shaders/%,$(RESOURCEDIR)/Shaders/%,$(wildcard ../Resources/Shaders/*.*))
RESOURCES += $(patsubst ../Resources/Textures/%,$(RESOURCEDIR)/Textures/%,$(wildcard ../Resources/Textures/*.*))
RESOURCES += $(RESOURCEDIR)/app.icns

all: $(CONTENTSDIR)/MacOS \
	$(CONTENTSDIR)/Info.pList \
	$(CONTENTSDIR)/MacOS/$(APPNAME) \
	$(RESOURCES)
  
$(CONTENTSDIR)/MacOS:
	mkdir -p $(CONTENTSDIR)/MacOS
  
$(CONTENTSDIR)/Info.pList: Info.plist
	cp -f $^ $@
	@chmod 444 $@

$(RESOURCEDIR)/app.icns: app.icns
	@mkdir -p $(@D)
	@chmod 755 $(@D)
	cp -f $^ $@
	@chmod 444 $@

$(RESOURCEDIR)/Fonts/%: ../Resources/Fonts/%
	@mkdir -p $(@D)
	@chmod 755 $(@D)
	cp -f $^ $@
	@chmod 444 $@

$(RESOURCEDIR)/Models/%: ../Resources/Models/%
	@mkdir -p $(@D)
	@chmod 755 $(@D)
	cp -f $^ $@
	@chmod 444 $@

$(RESOURCEDIR)/Shaders/%: ../Resources/Shaders/%
	@mkdir -p $(@D)
	@chmod 755 $(@D)
	cp -f $^ $@
	@chmod 444 $@

$(RESOURCEDIR)/Textures/%: ../Resources/Textures/%
	@mkdir -p $(@D)
	@chmod 755 $(@D)
	cp -f $^ $@
	@chmod 444 $@
  
$(CONTENTSDIR)/MacOS/$(APPNAME): $(OBJECTS) ../3rdparty/GLFW/libglfw3.1.2_mac.a ../3rdparty/GL/libGLEW_mac.a ../3rdparty/AntTweakBar/libAntTweakBar.a
	$(CPP) $(LDFLAGS) -o $@  $^

$(OBJECTDIR)/%.o: ../source/%.c ../include/*.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(OBJECTDIR)/%.o: ../source/%.cpp ../include/*.h
	@mkdir -p $(@D)
	$(CPP) $(CFLAGS) -o $@ $<

clean:
	rm -R -f ../build
  