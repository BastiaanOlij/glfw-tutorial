# Compiler directives for Windows
CFLAGS = /c /WX /MD /nologo /D "WIN32" /I..\include /I..\3rdparty
LDFLAGS = /nologo /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /WX 
WINLIBS = opengl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib 

APPNAME = glfw-tutorial.exe
OBJECTDIR = ..\build\Objects
CONTENTSDIR = ..\build

OBJECTS = $(OBJECTDIR)\main.obj $(OBJECTDIR)\engine.obj

all: $(CONTENTSDIR) \
  $(OBJECTDIR) \
  $(CONTENTSDIR)\$(APPNAME) \
  $(CONTENTSDIR)\glew32.dll \
  $(CONTENTSDIR)\DroidSerif-Regular.ttf \
  $(CONTENTSDIR)\boxtexture.jpg \
  $(CONTENTSDIR)\EarthMap.jpg \
  $(CONTENTSDIR)\tb_stars.png \
  $(CONTENTSDIR)\tb_refmap.png \
  $(CONTENTSDIR)\tie-bomber.mtl \
  $(CONTENTSDIR)\tie-bomber.obj \
  $(CONTENTSDIR)\flatcolor.fs \
  $(CONTENTSDIR)\textured.fs \
  $(CONTENTSDIR)\reflect.fs \
  $(CONTENTSDIR)\standard.vs
	
$(CONTENTSDIR): 
  mkdir $(CONTENTSDIR)	

$(OBJECTDIR):
  mkdir $(OBJECTDIR)
  
$(CONTENTSDIR)\$(APPNAME): $(OBJECTS) ..\3rdparty\GLFW\glfw3.1.2_win.lib ..\3rdparty\GL\glew32.lib
  link $(LDFLAGS) /out:$@ $** $(WINLIBS)

$(CONTENTSDIR)\glew32.dll: ..\3rdparty\GL\glew32.dll
  copy /B /Y $** $@

$(CONTENTSDIR)\DroidSerif-Regular.ttf: ..\resources\DroidSerif-Regular.ttf
  copy /B /Y $** $@

$(CONTENTSDIR)\boxtexture.jpg: ..\resources\boxtexture.jpg
  copy /B /Y $** $@

$(CONTENTSDIR)\EarthMap.jpg: ..\resources\EarthMap.jpg
  copy /B /Y $** $@

$(CONTENTSDIR)\tb_stars.png: ..\resources\tb_stars.png
  copy /B /Y $** $@

$(CONTENTSDIR)\tb_refmap.png: ..\resources\tb_refmap.png
  copy /B /Y $** $@
  
$(CONTENTSDIR)\tie-bomber.mtl: ..\resources\tie-bomber.mtl
  copy /B /Y $** $@

$(CONTENTSDIR)\tie-bomber.obj: ..\resources\tie-bomber.obj
  copy /B /Y $** $@
  
$(CONTENTSDIR)\flatcolor.fs: ..\resources\flatcolor.fs
  copy /B /Y $** $@

$(CONTENTSDIR)\textured.fs: ..\resources\textured.fs
  copy /B /Y $** $@

$(CONTENTSDIR)\reflect.fs: ..\resources\reflect.fs
  copy /B /Y $** $@

$(CONTENTSDIR)\standard.vs: ..\resources\standard.vs
  copy /B /Y $** $@

$(OBJECTDIR)\engine.obj: ..\source\engine.c ..\include\engine.h
  cl $(CFLAGS) /Fo$@ ..\source\engine.c

$(OBJECTDIR)\main.obj: ..\source\main.c ..\include\engine.h
  cl $(CFLAGS) /Fo$@ ..\source\main.c

clean:
  rmdir $(CONTENTSDIR) /s /q
	