# Compiler directives for Windows
CFLAGS = /c /WX /MD /nologo /D "WIN32" /I..\include /I..\3rdparty
LDFLAGS = /nologo /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /WX 
WINLIBS = opengl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib 

APPNAME = glfw-tutorial.exe
OBJECTDIR = ..\build\Objects
CONTENTSDIR = ..\build
RESOURCEDIR = $(CONTENTSDIR)\Resources

OBJECTS = $(OBJECTDIR)\main.obj $(OBJECTDIR)\setup.obj $(OBJECTDIR)\engine.obj

all: $(CONTENTSDIR) \
  $(OBJECTDIR) \
  $(CONTENTSDIR)\$(APPNAME) \
  $(CONTENTSDIR)\glew32.dll \
  $(CONTENTSDIR)\AntTweakBar.dll \
  $(RESOURCEDIR) \
  $(RESOURCEDIR)\Fonts \
  $(RESOURCEDIR)\Fonts\DroidSerif-Regular.ttf \
  $(RESOURCEDIR)\Textures \
  $(RESOURCEDIR)\Textures\boxtexture.jpg \
  $(RESOURCEDIR)\Textures\EarthMap.jpg \
  $(RESOURCEDIR)\Textures\tb_stars.png \
  $(RESOURCEDIR)\Textures\tb_refmap.png \
  $(RESOURCEDIR)\Textures\skymap.png \
  $(RESOURCEDIR)\Textures\heightfield.jpg \
  $(RESOURCEDIR)\Textures\Grass.jpg \
  $(RESOURCEDIR)\Models \
  $(RESOURCEDIR)\Models\tie-bomber.mtl \
  $(RESOURCEDIR)\Models\tie-bomber.obj \
  $(RESOURCEDIR)\Shaders \
  $(RESOURCEDIR)\Shaders\skybox.vs \
  $(RESOURCEDIR)\Shaders\skybox.fs \
  $(RESOURCEDIR)\Shaders\hmap.vs \
  $(RESOURCEDIR)\Shaders\hmap.fs \
  $(RESOURCEDIR)\Shaders\flatcolor.fs \
  $(RESOURCEDIR)\Shaders\textured.fs \
  $(RESOURCEDIR)\Shaders\reflect.fs \
  $(RESOURCEDIR)\Shaders\standard.vs
	
$(CONTENTSDIR): 
  mkdir $(CONTENTSDIR)	

$(OBJECTDIR):
  mkdir $(OBJECTDIR)

$(OBJECTDIR)\engine.obj: ..\source\engine.c ..\include\engine.h
  cl $(CFLAGS) /Fo$@ ..\source\engine.c

$(OBJECTDIR)\setup.obj: ..\source\setup.c ..\include\setup.h
  cl $(CFLAGS) /Fo$@ ..\source\setup.c

$(OBJECTDIR)\main.obj: ..\source\main.c ..\include\setup.h ..\include\engine.h
  cl $(CFLAGS) /Fo$@ ..\source\main.c

$(CONTENTSDIR)\$(APPNAME): $(OBJECTS) ..\3rdparty\GLFW\glfw3.1.2_win.lib ..\3rdparty\GL\glew32.lib ..\3rdparty\AntTweakBar\AntTweakBar.lib
  link $(LDFLAGS) /out:$@ $** $(WINLIBS)

$(CONTENTSDIR)\glew32.dll: ..\3rdparty\GL\glew32.dll
  copy /B /Y $** $@

$(CONTENTSDIR)\AntTweakBar.dll: ..\3rdparty\AntTweakBar\AntTweakBar.dll
  copy /B /Y $** $@

$(RESOURCEDIR):
  mkdir $(RESOURCEDIR)

$(RESOURCEDIR)\Fonts:
  mkdir $(RESOURCEDIR)\Fonts

$(RESOURCEDIR)\Fonts\DroidSerif-Regular.ttf: ..\resources\Fonts\DroidSerif-Regular.ttf
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures:
  mkdir $(RESOURCEDIR)\Textures

$(RESOURCEDIR)\Textures\boxtexture.jpg: ..\resources\boxtexture.jpg
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\EarthMap.jpg: ..\resources\EarthMap.jpg
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\tb_stars.png: ..\resources\tb_stars.png
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\tb_refmap.png: ..\resources\tb_refmap.png
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\skymap.png: ..\resources\skymap.png
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\heightfield.jpg: ..\resources\heightfield.jpg
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\Grass.jpg: ..\resources\Grass.jpg
  copy /B /Y $** $@

$(RESOURCEDIR)\Models:
  mkdir $(RESOURCEDIR)\Models

$(RESOURCEDIR)\Models\tie-bomber.mtl: ..\resources\tie-bomber.mtl
  copy /B /Y $** $@

$(RESOURCEDIR)\Models\tie-bomber.obj: ..\resources\tie-bomber.obj
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders:
  mkdir $(RESOURCEDIR)\Shaders

$(RESOURCEDIR)\Shaders\skybox.vs: ..\resources\skybox.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\skybox.fs: ..\resources\skybox.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap.vs: ..\resources\hmap.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap.fs: ..\resources\hmap.fs
  copy /B /Y $** $@
    
$(RESOURCEDIR)\Shaders\flatcolor.fs: ..\resources\flatcolor.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\textured.fs: ..\resources\textured.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\reflect.fs: ..\resources\reflect.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\standard.vs: ..\resources\standard.vs
  copy /B /Y $** $@

clean:
  rmdir $(CONTENTSDIR) /s /q
	