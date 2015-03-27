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
  $(CONTENTSDIR)\simple.fs \
  $(CONTENTSDIR)\simple.vs
	
$(CONTENTSDIR): 
  mkdir $(CONTENTSDIR)	

$(OBJECTDIR):
  mkdir $(OBJECTDIR)
  
$(CONTENTSDIR)\$(APPNAME): $(OBJECTS) ..\3rdparty\GLFW\glfw3.lib ..\3rdparty\GL\glew32.lib
  link $(LDFLAGS) /out:$@ $** $(WINLIBS)

$(CONTENTSDIR)\glew32.dll: ..\3rdparty\GL\glew32.dll
  copy /B /Y $** $@

$(CONTENTSDIR)\DroidSerif-Regular.ttf: ..\resources\DroidSerif-Regular.ttf
  copy /B /Y $** $@
	
$(CONTENTSDIR)\simple.fs: ..\resources\simple.fs
  copy /B /Y $** $@

$(CONTENTSDIR)\simple.vs: ..\resources\simple.vs
  copy /B /Y $** $@
  
$(OBJECTDIR)\engine.obj: ..\source\engine.c ..\include\engine.h
  cl $(CFLAGS) /Fo$@ ..\source\engine.c

$(OBJECTDIR)\main.obj: ..\source\main.c ..\include\engine.h
  cl $(CFLAGS) /Fo$@ ..\source\main.c

clean:
  rmdir ..\build /s /q
	