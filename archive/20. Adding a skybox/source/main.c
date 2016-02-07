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
#define TEXTURE_IMPLEMENTATION
#define SPRITE_IMPLEMENTATION
#define MATERIAL_IMPLEMENTATION
#define MESH_IMPLEMENTATION
#define JOYSTICK_IMPLEMENTATION

// Include our setup handler
#include "setup.h"

// Include our engine
#include "engine.h"

// for now make our window global to make our key handling easier
GLFWwindow* window;

// glfw version
void error_callback_glfw(int error, const char* description) {
  errorlog(error, description); 
};

////////////////////////////////////////////////////////////////////////////////////////
// GLFW callbacks

static void mouse_Callback(GLFWwindow* pWindow, int pButton, int pAction, int pMods) {
};

static void cursorPos_Callback(GLFWwindow* pWindow, double pX, double pY) {
};

static void cursorScroll_Callback(GLFWwindow* pWindow, double pXOffset, double pYOffset) {
};

static void key_callback(GLFWwindow* pWindow, int pKey, int pScanCode, int pAction, int pMods) {
  if (pKey == GLFW_KEY_ESCAPE && pAction == GLFW_PRESS) {
    glfwSetWindowShouldClose(pWindow, GL_TRUE);
  } else if (pAction == GLFW_PRESS) {
    // let our engine know the key was pressed and let it do what it needs to do..
    engineKeyPressed(pKey);    
  } else if (pAction == GLFW_REPEAT) {    
    // let our engine know the key was pressed and let it do what it needs to do..
    engineKeyPressed(pKey);
  } else if (pAction == GLFW_RELEASE) {
    // nothing to do here for now..
  };
};

static void char_Callback(GLFWwindow* pWindow, unsigned int pChar) {
};

////////////////////////////////////////////////////////////////////////////////////////
// callbacks from implementation

bool keypressed_callback(int pKey) {
  return glfwGetKey(window, pKey) == GLFW_PRESS;
};

////////////////////////////////////////////////////////////////////////////////////////
// and our main function

int main(void) {
  glfw_setup    info;
    
  // Just mark that we've been loaded
  errorlog(0, "GLFW Tutorial started");
  
  // tell GLFW how to inform us of issues
  glfwSetErrorCallback(error_callback_glfw);

  // see if we can initialize GLFW
  if (!glfwInit()) {
    exit(EXIT_FAILURE);    
  };
  
  if (!SetupGLFW(&info)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  };
  
  // make sure we're using OpenGL 3.2+
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  if (info.monitor != NULL) {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // don't resize this window
    glfwWindowHint(GLFW_RED_BITS, info.vidmode.redBits);
    glfwWindowHint(GLFW_GREEN_BITS, info.vidmode.greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, info.vidmode.blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, info.vidmode.refreshRate);
    if (info.stereomode == 2) {
      glfwWindowHint(GLFW_STEREO, GL_TRUE);    
    };

    errorlog(0, "Initializing fullscreen mode %i, %i - %i",info.vidmode.width, info.vidmode.height, info.stereomode);
    window = glfwCreateWindow(info.vidmode.width, info.vidmode.height, "GLFW Tutorial", info.monitor, NULL);
  } else {
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    errorlog(0, "Initializing windowed mode %i, %i - %i",info.vidmode.width, info.vidmode.height, info.stereomode);
    window = glfwCreateWindow(info.vidmode.width, info.vidmode.height, "GLFW Tutorial", NULL, NULL);
  };
  
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
    glfwSetMouseButtonCallback(window, mouse_Callback);
    glfwSetCursorPosCallback(window, cursorPos_Callback);
    glfwSetScrollCallback(window, cursorScroll_Callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_Callback);  
    
    // hide cursor if in full screen
    if (info.monitor != NULL) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    };
    
    // clear out our joystick info
    initJoystickInfo();
    
    // load and initialize our engine
    engineInit();
    engineSetErrorCallback(errorlog);
    engineSetKeyPressedCallback(keypressed_callback);
    engineLoad();

    // and start our render loop
    while (!glfwWindowShouldClose(window)) {
      int windowWidth, windowHeight, frameWidth, frameHeight;
      float ratio;

      // check our joysticks
      updateJoystickInfoGLFW();
      
      // update our stats
      engineUpdate(glfwGetTime());
      
      // get our window and frame buffer size, we'll move this into GLFWs window resize callbacks eventually
      glfwGetWindowSize(window, &windowWidth, &windowHeight);
      glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
      
      ratio = (float) frameWidth / (float) frameHeight;
      
      switch (info.stereomode) {
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

          // for now we ignore our UI in stereo mode
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
          
          // for now we ignore our UI in stereo mode
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
    TwTerminate();
    glfwDestroyWindow(window);
    window = NULL;
  };
  
  // lets be nice and cleanup
  engineUnload();
  
  // the end....
  glfwTerminate();
};




