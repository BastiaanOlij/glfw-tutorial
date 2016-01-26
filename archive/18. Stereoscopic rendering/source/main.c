// make sure we include any implementations here (note that these libraries are all included through engine.h)
#define FONTSTASH_IMPLEMENTATION
#define GLFONTSTASH_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define ERRORLOG_IMPLEMENTATION
#define VARCHAR_IMPLEMENTATION
#define LINKEDLIST_IMPLEMENTATION
#define DYNARRAY_IMPLEMENTATION
#define MATH3D_IMPLEMENTATION
#define SHADER_IMPLEMENTATION
#define TILEMAP_IMPLEMENTATION
#define SPRITE_IMPLEMENTATION
#define MATERIAL_IMPLEMENTATION
#define MESH_IMPLEMENTATION

// Uncomment to run full screen, we'll make this better some day
// #define GLFW_FULLSCREEN

// Include our engine
#include "engine.h"

// for now make our window global to make our key handling easier
GLFWwindow* window;
int stereo_mode = 1; // 0 is off, 1 is splitscreen, 2 = hardware (not yet tested)

// glfw version
void error_callback_glfw(int error, const char* description) {
  errorlog(error, description);	
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
  if (stereo_mode==2) {
    glfwWindowHint(GLFW_STEREO, GL_TRUE);    
  };
  
  // create our window (full screen for now is a compiler toggle, we'll make something nicer another day)
#ifdef GLFW_FULLSCREEN
  window = glfwCreateWindow(1024, 768, "GLFW Tutorial", glfwGetPrimaryMonitor(), NULL);
#else
  window = glfwCreateWindow(640, 480, "GLFW Tutorial", NULL, NULL);
#endif
  if (window == NULL) {
	  errorlog(-1, "Couldn''t construct GLFW window");
  } else {
    GLenum err;
	
    // make our context current
    glfwMakeContextCurrent(window);
 
    // init GLEW
    glewExperimental=true;
    err = glewInit();
    if (err != GLEW_OK) {
      errorlog(err, (char*) glewGetErrorString(err)); 
      exit(EXIT_FAILURE); 
    };

    // tell GLFW how to inform us of keyboard input
    glfwSetKeyCallback(window, key_callback);
    
    // load and initialize our engine
    engineInit();
    engineSetErrorCallback(errorlog);
    engineSetKeyPressedCallback(keypressed_callback);
    engineLoad();

    // and start our render loop
    while (!glfwWindowShouldClose(window)) {
      int windowWidth, windowHeight, frameWidth, frameHeight;
      float ratio;

      // update our stats
      engineUpdate(glfwGetTime());
      
      // get our window and frame buffer size, we'll move this into GLFWs window resize callbacks eventually
      glfwGetWindowSize(window, &windowWidth, &windowHeight);
      glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
      
      ratio = (float) frameWidth / (float) frameHeight;
      
      switch (stereo_mode) {
        case 1: {
          // clear our viewport
          glClearColor(0.1, 0.1, 0.1, 1.0);
          glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);      

          // render split screen 3D
          int halfWidth = frameWidth / 2;

          // set our viewport
          glViewport(0, 0, halfWidth, frameHeight);
      
          // and render left
          engineRender(halfWidth, frameHeight, ratio, 1);          
          
          // set our viewport
          glViewport(halfWidth, 0, halfWidth, frameHeight);
      
          // and render right
          engineRender(halfWidth, frameHeight, ratio, 2);          
        }; break;
        case 2: {
          // render hardware left/right buffers
          
          // clear our viewport
          glDrawBuffer(GL_BACK);
          glClearColor(0.1, 0.1, 0.1, 1.0);
          glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);                

          // set our viewport
          glViewport(0, 0, frameWidth, frameHeight);
          
          // render left
          glDrawBuffer(GL_BACK_LEFT);
          engineRender(frameWidth, frameHeight, ratio, 1);
               
          // render right
          glDrawBuffer(GL_BACK_RIGHT);
          engineRender(frameWidth, frameHeight, ratio, 2);          
        }; break;
        default: {
          // render normal...

          // clear our viewport
          glClearColor(0.1, 0.1, 0.1, 1.0);
          glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);      
          
          // set our viewport
          glViewport(0, 0, frameWidth, frameHeight);
      
          // and render
          engineRender(frameWidth, frameHeight, ratio, 0);          
        }; break;
      };
      
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




