/********************************************************
 * Setup is a container for our setup window
 * 
 * This library is geared to configuring our main GLFW
 * window.
 * 
 * it used AntTweakBar to render the interface
 * http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:howto
 ********************************************************/

#ifndef setuph
#define setuph

// include GLEW
#include <GL/glew.h>

// include these defines to let GLFW know we need OpenGL 3 support 
#ifndef GLFW_INCLUDE_GL_3
#define GLFW_INCLUDE_GL_3
#endif /* !GLFW_INCLUDE_GL_3 */
#include <GLFW/glfw3.h>

// include some standard libraries
#include <stdbool.h>

// our interface is handled by AntTweakBar
#include "AntTweakBar/AntTweakBar.h"

// and some of our internal stuff...
#include "errorlog.h"

// structure for maintaining our setup
typedef struct glfw_setup {
  GLFWmonitor * monitor;    /* NULL is windows, else selected monitor for full screen */
  GLFWvidmode   vidmode;    /* requested video mode, only width and height are initialized if monitor == NULL */
  int           stereomode; /* requested stereo mode, 0 = mono, 1 = splitscreen, 2 = left/right buffer, ignored if monitor == NULL */
} glfw_setup;

#ifdef __cplusplus
extern "C" {
#endif

bool SetupGLFW(glfw_setup * pInfo);

#ifdef __cplusplus
};
#endif

#endif /* !setuph */
