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
  $(RESOURCEDIR)\Textures\skybox.png \
  $(RESOURCEDIR)\Textures\heightfield.jpg \
  $(RESOURCEDIR)\Textures\Grass.jpg \
  $(RESOURCEDIR)\Textures\Leaves0018_1_S.png \
  $(RESOURCEDIR)\Textures\tb_refmap.png \
  $(RESOURCEDIR)\Textures\tb_stars.png \
  $(RESOURCEDIR)\Textures\tree.jpg \
  $(RESOURCEDIR)\Models \
  $(RESOURCEDIR)\Models\tie-bomber.mtl \
  $(RESOURCEDIR)\Models\tie-bomber.obj \
  $(RESOURCEDIR)\Models\tree.mtl \
  $(RESOURCEDIR)\Models\TreeLOD1.obj \
  $(RESOURCEDIR)\Models\TreeLOD2.obj \
  $(RESOURCEDIR)\Shaders \
  $(RESOURCEDIR)\Shaders\billboard.vs \
  $(RESOURCEDIR)\Shaders\billboard.fs \
  $(RESOURCEDIR)\Shaders\geomainpass.vs \
  $(RESOURCEDIR)\Shaders\geomainpass.fs \
  $(RESOURCEDIR)\Shaders\hmap.vs \
  $(RESOURCEDIR)\Shaders\hmap.fs \
  $(RESOURCEDIR)\Shaders\hmap_ts.vs \
  $(RESOURCEDIR)\Shaders\hmap_ts.gs \
  $(RESOURCEDIR)\Shaders\hmap_ts.te \
  $(RESOURCEDIR)\Shaders\hmap_ts.ts \
  $(RESOURCEDIR)\Shaders\hmap_ts.fs \
  $(RESOURCEDIR)\Shaders\outputs.fs \
  $(RESOURCEDIR)\Shaders\shadow.vs \
  $(RESOURCEDIR)\Shaders\shadow.fs \
  $(RESOURCEDIR)\Shaders\shadowmap.fs \
  $(RESOURCEDIR)\Shaders\skybox.vs \
  $(RESOURCEDIR)\Shaders\skybox.fs \
  $(RESOURCEDIR)\Shaders\standard.fs \
  $(RESOURCEDIR)\Shaders\standard.vs
	
$(CONTENTSDIR): 
  mkdir $(CONTENTSDIR)	

$(OBJECTDIR):
  mkdir $(OBJECTDIR)

$(OBJECTDIR)\engine.obj: ..\source\engine.c ..\include\*.h
  cl $(CFLAGS) /Fo$@ ..\source\engine.c

$(OBJECTDIR)\setup.obj: ..\source\setup.c ..\include\*.h
  cl $(CFLAGS) /Fo$@ ..\source\setup.c

$(OBJECTDIR)\main.obj: ..\source\main.c ..\include\*.h ..\include\engine.h
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

$(RESOURCEDIR)\Textures\Leaves0018_1_S.png: ..\resources\Textures\Leaves0018_1_S.png
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\tree.jpg: ..\resources\Textures\tree.jpg
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\tb_stars.png: ..\resources\Textures\tb_stars.png
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\tb_refmap.png: ..\resources\Textures\tb_refmap.png
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\skybox.png: ..\resources\Textures\skybox.png
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\heightfield.jpg: ..\resources\Textures\heightfield.jpg
  copy /B /Y $** $@

$(RESOURCEDIR)\Textures\Grass.jpg: ..\resources\Textures\Grass.jpg
  copy /B /Y $** $@

$(RESOURCEDIR)\Models:
  mkdir $(RESOURCEDIR)\Models

$(RESOURCEDIR)\Models\tie-bomber.mtl: ..\resources\Models\tie-bomber.mtl
  copy /B /Y $** $@

$(RESOURCEDIR)\Models\tie-bomber.obj: ..\resources\Models\tie-bomber.obj
  copy /B /Y $** $@

$(RESOURCEDIR)\Models\tree.mtl: ..\resources\Models\tree.mtl
  copy /B /Y $** $@

$(RESOURCEDIR)\Models\TreeLOD1.obj: ..\resources\Models\TreeLOD1.obj
  copy /B /Y $** $@  

$(RESOURCEDIR)\Models\TreeLOD2.obj: ..\resources\Models\TreeLOD2.obj
  copy /B /Y $** $@  
  
$(RESOURCEDIR)\Shaders:
  mkdir $(RESOURCEDIR)\Shaders

$(RESOURCEDIR)\Shaders\billboard.vs: ..\resources\Shaders\billboard.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\billboard.fs: ..\resources\Shaders\billboard.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\billboard.vs: ..\resources\Shaders\geomainpass.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\billboard.fs: ..\resources\Shaders\geomainpass.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap.vs: ..\resources\Shaders\hmap.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap.fs: ..\resources\Shaders\hmap.fs
  copy /B /Y $** $@
  
$(RESOURCEDIR)\Shaders\hmap_ts.vs: ..\resources\Shaders\hmap_ts.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap_ts.gs: ..\resources\Shaders\hmap_ts.gs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap_ts.te: ..\resources\Shaders\hmap_ts.te
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap_ts.ts: ..\resources\Shaders\hmap_ts.ts
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\hmap_ts.fs: ..\resources\Shaders\hmap_ts.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\shadow.vs: ..\resources\Shaders\output.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\shadow.vs: ..\resources\Shaders\shadow.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\shadow.fs: ..\resources\Shaders\shadow.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\shadowmap.fs: ..\resources\Shaders\shadowmap.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\skybox.vs: ..\resources\Shaders\skybox.vs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\skybox.fs: ..\resources\Shaders\skybox.fs
  copy /B /Y $** $@
    
$(RESOURCEDIR)\Shaders\standard.fs: ..\resources\Shaders\standard.fs
  copy /B /Y $** $@

$(RESOURCEDIR)\Shaders\standard.vs: ..\resources\Shaders\standard.vs
  copy /B /Y $** $@

clean:
  rmdir $(CONTENTSDIR) /s /q
	