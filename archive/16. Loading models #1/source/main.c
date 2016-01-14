// make sure we include any implementations here (note that these libraries are all included through engine.h)
#define FONTSTASH_IMPLEMENTATION
#define GLFONTSTASH_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define MATH3D_IMPLEMENTATION
#define SHADER_IMPLEMENTATION
#define TILEMAP_IMPLEMENTATION
#define SPRITE_IMPLEMENTATION
#define MESH_IMPLEMENTATION

// Uncomment to run full screen, we'll make this better some day
// #define GLFW_FULLSCREEN

// Include our engine
#include "engine.h"

// include some standard libraries
#ifdef __APPLE__
#include <syslog.h>
#endif

// for now make our window global to make our key handling easier
GLFWwindow* window;

// function for logging errors
void error_callback(int error, const char* description, ...) {
  va_list args;
  char fulldesc[2048];
  
  va_start(args, description);
  vsprintf(fulldesc, description, args);
  va_end(args);
  
#ifdef __APPLE__
	// output to syslog
	syslog(LOG_ALERT, "%i: %s", error, fulldesc);  
#elif WIN32
  // output to stderr
  fprintf(stderr, "%i: %s", error, fulldesc);
#else
  // not sure what we're doing on other platforms yet
#endif
};

// glfw version
void error_callback_glfw(int error, const char* description) {
#ifdef __APPLE__
	// output to syslog
	syslog(LOG_ALERT, "%i: %s", error, description);  
#elif WIN32
  // output to stderr
  fprintf(stderr, "%i: %s", error, description);
#else
  // not sure what we're doing on other platforms yet
#endif
};

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (action == GLFW_PRESS) {
    // let our engine know the key was pressed and let it do what it needs to do..
    engineKeyPressed(key);    
  } else if (action == GLFW_REPEAT) {    
    // let our engine know the key was pressed and let it do what it needs to do..
    engineKeyPressed(key);
  } else if (action == GLFW_RELEASE) {
    // nothing to do here for now..
  };
};

bool keypressed_callback(int pKey) {
  return glfwGetKey(window, pKey) == GLFW_PRESS;
};

int main(void) {  
  // tell GLFW how to inform us of issues
  glfwSetErrorCallback(error_callback_glfw);

  // see if we can initialize GLFW
  if (!glfwInit()) {
    exit(EXIT_FAILURE);    
  };
  
  // make sure we're using OpenGL 3.2+
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // create our window (full screen for now is a compiler toggle, we'll make something nicer another day)
#ifdef GLFW_FULLSCREEN
  window = glfwCreateWindow(1024, 768, "GLFW Tutorial", glfwGetPrimaryMonitor(), NULL);
#else
  window = glfwCreateWindow(640, 480, "GLFW Tutorial", NULL, NULL);
#endif
  if (window) {
    GLenum err;
	
    // make our context current
    glfwMakeContextCurrent(window);
 
    // init GLEW
    glewExperimental=true;
    err = glewInit();
    if (err != GLEW_OK) {
      error_callback(err, (char*) glewGetErrorString(err)); 
      exit(EXIT_FAILURE); 
    };

    // tell GLFW how to inform us of keyboard input
    glfwSetKeyCallback(window, key_callback);
    
    // load and initialize our engine
    engineInit();
    engineSetErrorCallback(error_callback);
    engineSetKeyPressedCallback(keypressed_callback);
    engineLoad();

    // and start our render loop
    while (!glfwWindowShouldClose(window)) {
      int windowWidth, windowHeight, frameWidth, frameHeight;

      // update our stats
      engineUpdate(glfwGetTime());
      
      // get our window and frame buffer size, we'll move this into GLFWs window resize callbacks eventually
      glfwGetWindowSize(window, &windowWidth, &windowHeight);
      glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
      
      // set and clear our viewport
      glViewport(0, 0, frameWidth, frameHeight);
      glClearColor(0.1, 0.1, 0.1, 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);      
      
      // and render
      engineRender(frameWidth, frameHeight);
      
      // swap our buffers around so the user sees our new frame
      glfwSwapBuffers(window);
      glfwPollEvents();
    };
  
    // close our window
    glfwDestroyWindow(window);
    window = NULL;
  };
  
  // lets be nice and cleanup
  engineUnload();
  
  // the end....
  glfwTerminate();
};
