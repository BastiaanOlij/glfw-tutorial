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

// include GLEW
#include <GL/glew.h>

// include these defines to let GLFW know we need OpenGL 3 support 
#define GLFW_INCLUDE_GL_3
#include <GLFW/glfw3.h>

// include some standard libraries

// include support libraries
#include "math3d.h"
#include "shaders.h"
#include "fontstash/fontstash.h"
#include "fontstash/gl3fontstash.h"
#include "stb/stb_image.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(* EngineError)(int, const char*);

void engineSetErrorCallback(EngineError pCallback);
void engineLoad();
void engineUnload();
void engineUpdate(double pSecondsPassed);
void engineRender(int pWidth, int pHeight);
void engineViewRotate(float pAngle);
void engineViewMove(float pX, float pY);

#ifdef __cplusplus
};
#endif
  
#endif