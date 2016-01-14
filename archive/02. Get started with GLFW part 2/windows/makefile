# Compiler directives for Windows
CFLAGS = /c /WX /MD /nologo /D "WIN32" /I..\include /I..\3rdparty
LDFLAGS = /nologo /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /WX 
WINLIBS = opengl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib 

APPNAME = glfw-tutorial.exe
OBJECTDIR = ..\build\Objects
CONTENTSDIR = ..\build

OBJECTS = $(OBJECTDIR)\main.obj

all: $(CONTENTSDIR) \
  $(OBJECTDIR) \
  $(CONTENTSDIR)\$(APPNAME) \
	
$(CONTENTSDIR): 
  mkdir $(CONTENTSDIR)	

$(OBJECTDIR):
  mkdir $(OBJECTDIR)
  
$(CONTENTSDIR)\$(APPNAME): $(OBJECTS) ..\3rdparty\GLFW\glfw3.lib 
  link $(LDFLAGS) /out:$@ $** $(WINLIBS)

$(OBJECTDIR)\main.obj:
  cl $(CFLAGS) /Fo$@ ..\source\main.c

clean:
  rmdir ..\build /s /q
	