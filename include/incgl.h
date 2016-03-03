/********************************************************
 * This is just a header file that includes our opengl
 * headers, in these case by including glew and glfw
 *
 * This makes it easy to change if we want to use our
 * opengl headers directly or use some other framework
 ********************************************************/

// include GLEW
#include <GL/glew.h>

// include these defines to let GLFW know we need OpenGL 3 support 
#ifndef GLFW_INCLUDE_GL_3
#define GLFW_INCLUDE_GL_3
#endif /* !GLFW_INCLUDE_GL_3 */
#include <GLFW/glfw3.h>
