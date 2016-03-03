/********************************************************
 * joysticks.h - joystick handler by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define JOYSTICK_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. 
 *
 * Revision history:
 * 0.1  28-01-2016  First version with basic functions
 *
 ********************************************************/

#ifndef joystickh
#define joystickh

// our libraries we need
#include "system.h"

#define MAX_JOYSTICKS 16

typedef struct joystickInfo {
  bool          enabled;
  char          name[25];
  int           axesCount;
  float         axes[16];
  int           buttonCount;
  unsigned char buttons[32];
} joystickInfo;


#ifdef __cplusplus
extern "C" {
#endif
  
const joystickInfo * getJoystickInfo(int pIndex);
void initJoystickInfo(void);
void updateJoystickInfoGLFW(void);

#ifdef __cplusplus
};
#endif 

#ifdef JOYSTICK_IMPLEMENTATION

// support for up to MAX_JOYSTICKS joysticks
joystickInfo Joysticks[MAX_JOYSTICKS];

// get info about a joystick
const joystickInfo * getJoystickInfo(int pIndex) {
  if (pIndex < MAX_JOYSTICKS) {
    return &Joysticks[pIndex];            
  } else {
    return NULL;
  };
};

// clear our joystick info
void initJoystickInfo(void) {
  int           i;

  for (i = 0; i < MAX_JOYSTICKS; i++) {
    Joysticks[i].enabled = false;
    strcpy(Joysticks[i].name, "None");
    Joysticks[i].axesCount = 0;
    memset(Joysticks[i].axes, 0, sizeof(float) * 16);
    Joysticks[i].buttonCount = 0;
    memset(Joysticks[i].buttons, 0, sizeof(unsigned char) * 32);
  };
};

// implementation for updating joystick information through GLFW
void updateJoystickInfoGLFW(void) {
  int           i;

  for (i = 0; i < MAX_JOYSTICKS; i++) {
    bool present = glfwJoystickPresent(GLFW_JOYSTICK_1 + i) == GL_TRUE;
    if (Joysticks[i].enabled != present) {
      Joysticks[i].enabled = present;
      if (present) {
        strcpy(Joysticks[i].name, glfwGetJoystickName(GLFW_JOYSTICK_1 + i));
        errorlog(i, "Found joystick %s", Joysticks[i].name);
      } else {
        strcpy(Joysticks[i].name, "None");
        Joysticks[i].axesCount = 0;
        memset(Joysticks[i].axes, 0, sizeof(float) * 16);
        Joysticks[i].buttonCount = 0;
        memset(Joysticks[i].buttons, 0, sizeof(unsigned char) * 32);          
      };
    };
  
    if (present) {
      // always update these
      const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &Joysticks[i].axesCount);
      const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + i, &Joysticks[i].buttonCount);
    
      memcpy(Joysticks[i].axes, axes, sizeof(float) * Joysticks[i].axesCount);
      memcpy(Joysticks[i].buttons, buttons, sizeof(unsigned char) * Joysticks[i].buttonCount);          
    };
  };
};

#endif /* JOYSTICK_IMPLEMENTATION */

#endif /* !joystickh */