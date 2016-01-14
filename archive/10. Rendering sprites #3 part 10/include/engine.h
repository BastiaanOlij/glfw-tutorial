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

#define SPRITE_SCALE 3.0

// enumerations
enum texture_types {
  TEXT_MAPDATA,
  TEXT_TILEDATA,
  TEXT_SPRITEDATA,
  TEXT_COUNT
};

enum conrad_animations {
  CR_ANIM_LOOK_LEFT,
  CR_ANIM_TURN_LEFT_TO_RIGHT,
  CR_ANIM_LOOK_RIGHT,
  CR_ANIM_TURN_RIGHT_TO_LEFT,

  CR_ANIM_WALK_RIGHT_1,
  CR_ANIM_STOP_WALK_RIGHT,
  CR_ANIM_WALK_RIGHT_2,

  CR_ANIM_WALK_LEFT_1,
  CR_ANIM_STOP_WALK_LEFT,
  CR_ANIM_WALK_LEFT_2,
  
  CR_ANIM_GET_DOWN_RIGHT,
  CR_ANIM_STAY_DOWN_RIGHT,
  CR_ANIM_ROLL_RIGHT,
  CR_ANIM_REVROLL_RIGHT,
  CR_ANIM_GET_UP_RIGHT_1,
  CR_ANIM_GET_UP_RIGHT_2,

  CR_ANIM_GET_DOWN_LEFT,
  CR_ANIM_STAY_DOWN_LEFT,
  CR_ANIM_ROLL_LEFT,
  CR_ANIM_REVROLL_LEFT,
  CR_ANIM_GET_UP_LEFT_1,
  CR_ANIM_GET_UP_LEFT_2,
  
  CR_ANIM_JUMP_UP_RIGHT,
  CR_ANIM_JUST_HANGING_RIGHT,
  CR_ANIM_COME_DOWN_RIGHT,
  CR_ANIM_CLIMB_UP_RIGHT,

  CR_ANIM_JUMP_UP_LEFT,
  CR_ANIM_JUST_HANGING_LEFT,
  CR_ANIM_COME_DOWN_LEFT,
  CR_ANIM_CLIMB_UP_LEFT,

  CR_ANIM_COUNT
};
  
// structure to record information about an animation
typedef struct animation {
  GLint     firstSprite;            // first sprite of animation
  GLint     lastSprite;             // last sprite of animation
  bool      flip;                   // flip sprite horizontally
  GLfloat   moveX;                  // add this to our X after anim is finished
  GLfloat   moveY;                  // add this to our Y after anim is finished  
} animation;

// structure to control our characters with
typedef struct action_map {
  GLint     animationEnding;        // when this animation ends
  GLint     startAnimation;         // start this animation
  int       keys[5];                // if these keys are pressed (ignoring zeroes)
} action_map;

typedef void(* EngineError)(int, const char*, ...);
typedef bool(* EngineKeyPressed)(int);

void engineSetErrorCallback(EngineError pCallback);
void engineSetKeyPressedCallback(EngineKeyPressed pCallback);
void engineLoad();
void engineUnload();
void engineUpdate(double pSecondsPassed);
void engineRender(int pWidth, int pHeight);
void engineKeyPressed(int pKey);

#ifdef __cplusplus
};
#endif
  
#endif