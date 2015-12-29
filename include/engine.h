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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// include support libraries
#include "math3d.h"
#include "shaders.h"
#include "fontstash/fontstash.h"
#include "fontstash/gl3fontstash.h"
#include "stb/stb_image.h"
#include "tilemap.h"
#include "spritesheet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPRITE_SCALE 1.0f
#define SPRITE_X_CENTER 16.0f
#define SPRITE_Y_CENTER 0.0f
#define TILE_SCALE 32.0f

// enumerations
enum texture_types {
  TEXT_MAPDATA,
  TEXT_TILEDATA,
  TEXT_SPRITEDATA,
  TEXT_COUNT
};

// structure to check value of relative tile
typedef struct move_map {
  GLint     relX;                       // check horizontal tiles
  GLint     relY;                       // check vertical tiles
  unsigned char *tiles;                 // tile types to check
} move_map;

// structure to control our follow up actions
typedef struct action_map {
  GLint     startAnimation;             // start this animation
  int       *keys;                      // if these keys are pressed (zero terminates)
  move_map  *moveMap;                   // our move map checks
} action_map;
  
// structure to record information about an animation
typedef struct animation {
  GLint         firstSprite;            // first sprite of animation
  GLint         lastSprite;             // last sprite of animation
  bool          flip;                   // flip sprite horizontally
  GLfloat       moveX;                  // add this to our X after anim is finished
  GLfloat       moveY;                  // add this to our Y after anim is finished
  action_map    *followUpActions;       // follow up actions
} animation;

typedef void(* EngineError)(int, const char*, ...);
typedef bool(* EngineKeyPressed)(int);

void engineSetErrorCallback(EngineError pCallback);
void engineSetKeyPressedCallback(EngineKeyPressed pCallback);
void engineInit();
void engineLoad();
void engineUnload();
void engineUpdate(double pSecondsPassed);
void engineRender(int pWidth, int pHeight);
void engineKeyPressed(int pKey);

#ifdef __cplusplus
};
#endif
  
#endif