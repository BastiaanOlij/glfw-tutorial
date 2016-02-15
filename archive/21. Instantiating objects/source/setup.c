/********************************************************
 * Setup is a container for our setup window
 * 
 * This library is geared to configuring our main GLFW
 * window.
 * 
 * it used AntTweakBar to render the interface
 * http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:howto
 ********************************************************/

#include "setup.h"

typedef struct setupMonitorData {
  char          name[250];  // name of our monitor, assume 250 characters is enough...
  GLFWmonitor * monitor;
} setupMonitorData;

typedef struct setupVidmodeData {
  char          name[250];  
  GLFWvidmode   vidmode;
} setupVidmodeData;

typedef struct setupWindowData {
  GLFWwindow * window;
  bool         success;
  int          monitorIndex;
  int          vidmodeIndex;
} setupWindowData;

void TW_CALL Button_Callback(setupWindowData * pData) {
  pData->success = true;
  glfwSetWindowShouldClose(pData->window, GL_TRUE);
};

static void SetupMouse_Callback(GLFWwindow* pWindow, int pButton, int pAction, int pMods) {
  // AntTweakBar was based on GLFW 2.x so need to map to 3.x calls
  TwEventMouseButtonGLFW(pButton, pAction);
};

static void SetupCursorPos_Callback(GLFWwindow* pWindow, double pX, double pY) {
  // AntTweakBar was based on GLFW 2.x so need to map to 3.x calls
  TwEventMousePosGLFW(pX, pY);
};

static void SetupCursorScroll_Callback(GLFWwindow* pWindow, double pXOffset, double pYOffset) {
  // AntTweakBar was based on GLFW 2.x so need to map to 3.x calls
  TwEventMouseWheelGLFW(pXOffset);
};

static void SetupKey_Callback(GLFWwindow* pWindow, int pKey, int pScancode, int pAction, int pMods) {
  // AntTweakBar was based on GLFW 2.x so need to map to 3.x calls
  TwEventKeyGLFW(pKey, pAction);
};

static void SetupChar_Callback(GLFWwindow* pWindow, unsigned int pChar) {
  // This one doesn't seem mapable...
  // TwEventCharGLFW(pChar, ???);
};

setupMonitorData * SetupGetMonitors(int * pCount) {
  int                 i;
  setupMonitorData *  monArray = NULL;
  GLFWmonitor**       monitors;
  
  monitors = glfwGetMonitors(pCount);
  if ((monitors != NULL) && (*pCount > 0)) {
    // allocate enough space to hold our monitor info + our windowed entry
    monArray = (setupMonitorData *) malloc(sizeof(setupMonitorData) * (*pCount + 1));
    if (monArray != NULL) {            
      for (i = 0; i < *pCount; i++) {
          monArray[i].monitor = monitors[i];
          // The GLFW documentation is not clear about the lifecycle of the pointer returned by glfwGetMonitorName, so we copy it.
          strcpy(monArray[i].name, glfwGetMonitorName(monitors[i])); 
      };

      // add windowed
      monArray[*pCount].monitor = NULL;
      strcpy(monArray[*pCount].name, "Windowed");
      *pCount = *pCount + 1;
    };
  };
  
  return monArray;
};

setupVidmodeData * SetupGetVideoModes(GLFWmonitor* pMonitor, int * pCount) {
  int                   i, m=0;
  setupVidmodeData *    vidArray;
  const GLFWvidmode *   modes;
  
  modes = glfwGetVideoModes(pMonitor, pCount);
  
  if ((modes != NULL) && (*pCount > 0)) {
    vidArray = (setupVidmodeData *) malloc(sizeof(setupVidmodeData) * (*pCount));
    if (vidArray != NULL) {
      for (i = (*pCount) - 1; i>=0; i--) {
        sprintf(vidArray[m].name, "%ix%i %i/%i/%i %ihz", modes[i].width, modes[i].height, modes[i].redBits, modes[i].greenBits, modes[i].blueBits, modes[i].refreshRate);
        memcpy(&(vidArray[m].vidmode), &modes[i], sizeof(GLFWvidmode));
        m++;
      };
    };
  };
  
  return vidArray;
};

TwType TwDefineEnumFromArray(const char * pName, const void * pData, unsigned int pSize, unsigned int pCount, unsigned int pOffset) {
  TwType        newtype = 0;
  TwEnumVal *   twenums = (TwEnumVal *) malloc(sizeof(TwEnumVal) * pCount);
  int           i, pos = pOffset;
  
  if (twenums != NULL) {
    for (i = 0; i < pCount; i++) {
      twenums[i].Value = i;
      twenums[i].Label = ((char *)pData) + pos;
      pos += pSize;
    };
    
    newtype = TwDefineEnum(pName, twenums, pCount);
    
    // no longer need our array..
    free(twenums);
  };
  
  return newtype;
};

bool SetupGLFW(glfw_setup * pInfo) {
  setupWindowData     data;
  TwBar *             myBar;
  const GLFWvidmode * defaultMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int                 monitorCount = 0;
  setupMonitorData *  monitorArray = NULL;
  int                 vidmodeCount = 0;
  setupVidmodeData *  vidmodeArray = NULL;
  TwType              monitorType, vidmodeType, stereomodeType;
  
  // init our data structure
  data.window = NULL;
  data.monitorIndex = 0;
  data.vidmodeIndex = 0;
  data.success = false;
    
  // init with defaults
  pInfo->monitor = NULL;
  pInfo->vidmode.width = 640;
  pInfo->vidmode.height = 480;
  if (defaultMode == NULL) {
    pInfo->vidmode.redBits = 8;
    pInfo->vidmode.greenBits = 8;
    pInfo->vidmode.blueBits = 8;
    pInfo->vidmode.refreshRate = 60;
  } else {
    pInfo->vidmode.redBits = defaultMode->redBits;
    pInfo->vidmode.greenBits = defaultMode->greenBits;
    pInfo->vidmode.blueBits = defaultMode->blueBits;
    pInfo->vidmode.refreshRate = defaultMode->refreshRate;
  };
  pInfo->stereomode = 0;
  
  // start by creating a normal GLFW window to display our setup in
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // don't resize this window
  data.window = glfwCreateWindow(450, 200, "Setup", NULL, NULL);
  if (data.window == NULL) {
      return false;
  };
    
  // setup callbacks to bind GLFW to AntTweakBar
  glfwSetMouseButtonCallback(data.window, SetupMouse_Callback);
  glfwSetCursorPosCallback(data.window, SetupCursorPos_Callback);
  glfwSetScrollCallback(data.window, SetupCursorScroll_Callback);
  glfwSetKeyCallback(data.window, SetupKey_Callback);
  glfwSetCharCallback(data.window, SetupChar_Callback);  
  
  // make our context current
  glfwMakeContextCurrent(data.window);

  // Init AntTweakBar
  TwInit(TW_OPENGL, NULL); 
  TwWindowSize(450, 200);
  myBar = TwNewBar("Setup");
  TwDefine(" Setup position='10 10' size='430 180' iconifiable=false movable=false resizable=false valueswidth=225 buttonalign=center ");
  
  // Add monitor selection
  monitorArray = SetupGetMonitors(&monitorCount);
  if (monitorArray != NULL) {
    monitorType = TwDefineEnumFromArray("MonitorType", monitorArray, sizeof(setupMonitorData), monitorCount, 0);
    TwAddVarRW(myBar, "Monitor", monitorType, &data.monitorIndex, NULL);
  };
    
  // Add our resolution dropdown but without any variables
  vidmodeType = TwDefineEnum("VidmodeType", NULL, 0);
  TwAddVarRW(myBar, "Resolution", vidmodeType, &data.vidmodeIndex, NULL);
  TwDefine(" Setup/Resolution visible = false ");
  
  // add our stereo mode dropdown
  stereomodeType = TwDefineEnumFromString("StereomodeType", "2D,3D splitscreen");
  TwAddVarRW(myBar,"Stereo mode", stereomodeType, &pInfo->stereomode, NULL);
  
  // add our success button
  TwAddSeparator(myBar, "Sep1", NULL);
  TwAddButton(myBar, "StartButton", Button_Callback, &data, " label='Start program'  ");
  
  // and start our render loop
  while (!glfwWindowShouldClose(data.window)) {
    int frameWidth, frameHeight;
    
    // check for changes
    if (monitorArray != NULL) {
      if (pInfo->monitor != monitorArray[data.monitorIndex].monitor) {
        errorlog(0,"Changed monitor to %s",monitorArray[data.monitorIndex].name);
        pInfo->monitor = monitorArray[data.monitorIndex].monitor;
        data.vidmodeIndex = 0;
        
        if (vidmodeArray != NULL) {
          free(vidmodeArray);
          vidmodeArray = NULL;
        };
        
        if (pInfo->monitor == NULL) {
          TwDefine(" Setup/Resolution visible = false ");
          stereomodeType = TwDefineEnumFromString("StereomodeType", "2D,3D splitscreen");
        } else {
          vidmodeArray = SetupGetVideoModes(pInfo->monitor, &vidmodeCount);
          if (vidmodeArray != NULL) {
            vidmodeType = TwDefineEnumFromArray("VidmodeType", vidmodeArray, sizeof(setupVidmodeData), vidmodeCount, 0);
          };
          
          stereomodeType = TwDefineEnumFromString("StereomodeType", "2D,3D splitscreen,3D left/right buffer");
        
          TwDefine(" Setup/Resolution visible = true ");
        };
      };
      
      // Note that we do not update our vidmode data for the selected mode if applicable until the user presses OK
    };
        
    // get size info
    glfwGetFramebufferSize(data.window, &frameWidth, &frameHeight);  
    TwWindowSize(frameWidth, frameHeight);
    
    // clear our viewport
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);      
          
    // set our viewport
    glViewport(0, 0, frameWidth, frameHeight);
    
    // and draw
    TwDraw();
  
    // swap our buffers around so the user sees our new frame
    glfwSwapBuffers(data.window);
    glfwPollEvents();
  };
  
  // close our window
  TwTerminate();
  glfwDestroyWindow(data.window);
  data.window = NULL;
  
  if ((pInfo->monitor != NULL) && (vidmodeArray != NULL)) {
    memcpy(&(pInfo->vidmode), &(vidmodeArray[data.vidmodeIndex].vidmode), sizeof(GLFWvidmode));
  };
  
  // and cleanup
  if (monitorArray != NULL) {
    free(monitorArray);
    monitorArray = NULL;
  };
  if (vidmodeArray != NULL) {
    free(vidmodeArray);
    vidmodeArray = NULL;
  };
 
  return data.success;
};