/********************************************************
 * Engine is our container for our main program logic
 * 
 * This allows us to only place code in main.c that
 * interacts with the user through our framework and
 * thus change framework if needed.
 * 
 * We do still include GLEW and GLFW here but we may
 * eventually switch things out depending on our framework
 * 
 ********************************************************/

#ifndef engineh
#define engineh

// include opengl
#include "incgl.h"

// include some standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// and some more 3rd party librarys
#include <fontstash/fontstash.h>
#include <fontstash/gl3fontstash.h>

// include support libraries
#include "system.h"
#include "dynamicarray.h"
#include "linkedlist.h"
#include "varchar.h"
#include "math3d.h"
#include "texturemap.h"
#include "shaders.h"
#include "material.h"
// #include "tilemap.h"
// #include "spritesheet.h"
#include "mesh3d.h"
#include "meshnode.h"
#include "gbuffer.h"

#include "joysticks.h"

// enumerations

#ifdef __cplusplus
extern "C" {
#endif

typedef void(* EngineError)(int, const char*, ...);
typedef bool(* EngineKeyPressed)(int);

void engineSetKeyPressedCallback(EngineKeyPressed pCallback);
void engineInit();
void engineLoad(bool pHMD);
void engineUnload();
void engineUpdate(double pSecondsPassed);
void engineRender(int pWidth, int pHeight, float ratio, int pMode);
void engineKeyPressed(int pKey);

#ifdef __cplusplus
};
#endif
  
#endif