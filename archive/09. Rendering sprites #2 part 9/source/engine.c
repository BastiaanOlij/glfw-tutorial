/********************************************************
 * Engine is our container for our main program logic
 * 
 * This allows us to only place code in main.c that
 * interacts with the user through our framework and
 * thus change framework if needed.
 * 
 ********************************************************/

#include "engine.h"

//////////////////////////////////////////////////////////
// For now we just some global state to make life easy

EngineError engineErrCallback = NULL;

// shader program and buffers
newtileshader(ts);
newspritesheet(sp);

GLuint VAO = 0;
GLuint textures[TEXT_COUNT] = { 0, 0, 0 };

// and some globals for our fonts
FONScontext *	fs = NULL;
int font = FONS_INVALID;
float lineHeight = 0.0f;

// our view matrix
mat4  view;

// keeping track of keypresses
int keyStack[10];
int keyStackSize = 0;

int popKey() {
  int i, key;
  
  if (keyStackSize == 0) {
    key = 0;
  } else {
    key = keyStack[0];
    keyStackSize--;
    for (i = 0; i <= keyStackSize; i++) {
      keyStack[i] = keyStack[i+1];
    };
    
//    engineErrCallback(key, "Pop key");    
  };
  
  return key;
};

void pushKey(int pKey) {
  if (keyStackSize == 10) {
    // our stack is full, discard one!
    popKey();
  };
  
//  engineErrCallback(pKey, "Pushed key");
  
  keyStack[keyStackSize] = pKey;
  keyStackSize++;
};

// and some runtime variables.
double frames = 0.0f;
double fps = 0.0f;
double lastframes = 0.0f;
double lastsecs = 0.0f;

// map data
unsigned char mapdata[1600] = {
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,13,14,15,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,45,13,14,15,29,30,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,13,14,15,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,30,21,22,23,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29, 0, 1, 2,29,29,30,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,24,25,25,25,25,25,25,25,25,25,25,25,26, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,39,29,29,29,29,32,33,35,41,36,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,32,33,34,29,32,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,37,29,29,32,33,34,29,32,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,47,37,29,29,29,29,32,33,43,25,44,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,39,29,29,29,39,29,29,29,32,33,33,33,33,33,33,33,35,41,36,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  39,29,29,29,29,39,37,29,29,37,29,32,33,33,33,33,33,33,33,43,25,44,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,38,29,29,29,37,29,39,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,38,29,29,29,29,29,29,29,40,41,41,41,41,41,41,41,41,41,41,41,42, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
   6, 6, 7, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,28, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  14,14,15, 8, 9, 9, 9, 9, 9, 9, 9, 9,10, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  22,22,23,16,17,17,17,17,17,17,17,17,18,16,17,17,17,17,17,17,17,17,17,17,17,17,18,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,38,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,30,
  29,29,29,29,38,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,
  38,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,24,25,25,25,25,25,25,25,25,25,25,25,26,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,45,29,32,33,33,33,33,33,33,33,33,35,41,36,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,34,47,32,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,46,32,33,33,33,33,33,33,33,33,34,47,32,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,38,29,29,29,32,33,33,33,33,33,33,33,33,34,47,32,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,43,25,44,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,46,29,32,33,33,33,33,33,33,33,33,33,33,33,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34,47,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,40,41,41,41,41,41,41,41,41,41,41,41,42,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,29,29,29,30,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29
};

// sprite sheet data for conrad
sprite conradSpriteSheet[] = {
// row 1
     0.0,   0.0, 20.0, 78.0,   2.0, -39.0,  //  0 -- turn from looking left
    20.0,   0.0, 26.0, 78.0,   2.0, -39.0,  //  1
    46.0,   0.0, 26.0, 78.0,   2.0, -39.0,  //  2
    71.0,   0.0, 26.0, 78.0,   2.0, -39.0,  //  3
    96.0,   0.0, 27.0, 78.0,   0.0, -39.0,  //  4
   123.0,   0.0, 26.0, 78.0,   0.0, -39.0,  //  5
   148.0,   0.0, 22.0, 78.0,  -1.0, -39.0,  //  6 -- to looking right
   
   169.0,   0.0, 28.0, 78.0,  -3.0, -39.0,  //  7 -- stop walking right from 15
   198.0,   0.0, 26.0, 78.0,  -3.0, -39.0,  //  8
   224.0,   0.0, 22.0, 78.0,  -1.0, -39.0,  //  9 
   246.0,   0.0, 18.0, 78.0,  -1.0, -39.0,  // 10 -- finish stop walking right
   
   264.0,   0.0, 26.0, 78.0,  -3.0, -39.0,  // 11 -- walking right (left foot forward)
   292.0,   0.0, 28.0, 78.0,   4.0, -39.0,  // 12 
   324.0,   0.0, 42.0, 78.0,  11.0, -39.0,  // 13 
   368.0,   0.0, 46.0, 78.0,  13.0, -39.0,  // 14
   413.0,   0.0, 40.0, 80.0,  17.0, -40.0,  // 15 ---- +28.0 (+22 if we stop walking?)
   452.0,   0.0, 30.0, 80.0,  -5.0, -40.0,  // 16 -- (right foot forward) 
   482.0,   0.0, 26.0, 80.0,   3.0, -40.0,  // 17
   508.0,   0.0, 38.0, 80.0,  11.0, -40.0,  // 18
   546.0,   0.0, 48.0, 80.0,  16.0, -40.0,  // 19
   594.0,   0.0, 46.0, 80.0,  15.0, -40.0,  // 20
   640.0,   0.0, 36.0, 82.0,  20.0, -41.0,  // 21
   676.0,   0.0, 30.0, 82.0,  25.0, -41.0,  // 22 -- finished walking right +28.0
   
   706.0,   0.0, 28.0, 82.0,   0.0, -41.0,  // 23 (same as 11?)
    
// row 2  
     0.0,  96.0, 20.0, 78.0,   0.0, -39.0,  // 24 -- get down (facing left)
    20.0,  96.0, 22.0, 78.0,  -4.0, -39.0,  // 25
    42.0,  96.0, 26.0, 78.0,  -2.0, -39.0,  // 26
    68.0,  96.0, 28.0, 78.0,  -1.0, -39.0,  // 27
    96.0,  96.0, 30.0, 78.0,   0.0, -39.0,  // 28
   126.0,  96.0, 30.0, 78.0,   0.0, -39.0,  // 29
   156.0,  96.0, 30.0, 78.0,   0.0, -39.0,  // 30
   186.0,  96.0, 30.0, 78.0,   0.0, -39.0,  // 31 -- finish get down + 0.0
   
   216.0,  96.0, 70.0, 78.0, -20.0, -39.0,  // 32 -- start roll to the left
   286.0,  96.0, 80.0, 78.0, -28.0, -39.0,  // 33
   366.0,  96.0, 70.0, 80.0, -30.0, -40.0,  // 34
   436.0,  96.0, 60.0, 80.0, -44.0, -40.0,  // 35
   496.0,  96.0, 58.0, 80.0, -54.0, -40.0,  // 36
   554.0,  96.0, 50.0, 80.0, -66.0, -40.0,  // 37
   604.0,  96.0, 42.0, 80.0, -70.0, -40.0,  // 38
   646.0,  96.0, 42.0, 80.0, -76.0, -40.0,  // 39
   688.0,  96.0, 44.0, 80.0, -82.0, -40.0,  // 40
   732.0,  96.0, 44.0, 80.0, -82.0, -40.0,  // 41 --

// row 3  
     0.0, 188.0, 40.0, 76.0, -88.0, -38.0,  // 42
    40.0, 188.0, 48.0, 76.0, -94.0, -38.0,  // 43
    88.0, 188.0, 48.0, 78.0,-100.0, -39.0,  // 44
   136.0, 188.0, 48.0, 80.0,-106.0, -40.0,  // 45 -- finish roll -106.0 (-86.0 if we're getting up?)
   184.0, 188.0, 46.0, 80.0,   2.0, -40.0,  // 46 -- get up (facing left)
   230.0, 188.0, 36.0, 80.0, -14.0, -40.0,  // 47
   266.0, 188.0, 32.0, 80.0, -18.0, -40.0,  // 48
   298.0, 188.0, 28.0, 80.0, -20.0, -40.0,  // 49 -- finish get up -20.0
   326.0, 188.0, 26.0, 80.0,   0.0, -40.0,  // 50 -- jump up
   352.0, 188.0, 21.0, 80.0,   0.0, -40.0,  // 51
   373.0, 188.0, 22.0, 80.0,   0.0, -40.0,  // 52
   395.0, 188.0, 25.0, 80.0,   0.0, -40.0,  // 53
   420.0, 188.0, 28.0, 80.0,   0.0, -40.0,  // 54
   448.0, 188.0, 32.0, 80.0,   0.0, -40.0,  // 55
   480.0, 188.0, 32.0, 80.0,   0.0, -40.0,  // 56
   512.0, 188.0, 28.0, 80.0,   0.0, -40.0,  // 57
   540.0, 188.0, 30.0, 80.0,   0.0, -40.0,  // 58
   570.0, 188.0, 32.0, 80.0,   0.0, -40.0,  // 59
   602.0, 188.0, 34.0, 80.0,   0.0, -40.0,  // 60
   636.0, 188.0, 32.0, 80.0,   0.0, -40.0,  // 61
   668.0, 188.0, 29.0, 80.0,   0.0, -40.0,  // 62
   697.0, 188.0, 32.0, 80.0,   0.0, -40.0,  // 63
   729.0, 188.0, 31.0, 80.0,   0.0, -40.0,  // 64
   760.0, 182.0, 28.0, 86.0,   0.0, -43.0,  // 65 
   
// row 4   -- 364 - 90
     0.0, 274.0, 30.0, 90.0,   0.0, -45.0,  // 66 -- in the air (if we grab something here we can switch to climb)
    30.0, 274.0, 36.0, 90.0,   0.0, -45.0,  // 67 -- coming back down
    66.0, 274.0, 30.0, 92.0,   0.0, -46.0,  // 68
    96.0, 282.0, 26.0, 84.0,   0.0, -42.0,  // 69
   126.0, 298.0, 26.0, 68.0,   0.0, -34.0,  // 70 
   156.0, 298.0, 28.0, 68.0,   0.0, -34.0,  // 71
   186.0, 298.0, 30.0, 68.0,   0.0, -34.0,  // 72
   220.0, 298.0, 28.0, 68.0,   0.0, -34.0,  // 73
   250.0, 298.0, 26.0, 68.0,   0.0, -34.0,  // 74
   278.0, 298.0, 28.0, 68.0,   0.0, -34.0,  // 75
   308.0, 298.0, 26.0, 70.0,   0.0, -35.0,  // 76
   336.0, 296.0, 22.0, 72.0,   0.0, -35.0,  // 77
   364.0, 290.0, 18.0, 78.0,   0.0, -39.0,  // 78

// need to complete this...

};

// now define our animations
animation conradAnim[] = {
    0,    0,  false,   0.0,  0.0,    //  0 - look left
    0,    6,  false,   0.0,  0.0,    //  1 - turn from looking left to looking right
    6,    6,  false,   0.0,  0.0,    //  2 - look right
    6,    0,  false,   0.0,  0.0,    //  3 - turn from looking right to looking left
    
   11,   15,  false,  28.0,  0.0,    //  4 - walk right (left foot)
    7,   10,  false,   2.0,  0.0,    //  5 - stop walking right (after left foot)
   16,   22,  false,  28.0,  0.0,    //  6 - walk right (right foot)
   
   11,   15,   true, -28.0,  0.0,    //  7 - walk left (left foot, uhm or right foot, mirrored you know)
    7,   10,   true,  -2.0,  0.0,    //  8 - stop walking left (after left foot)
   16,   22,   true, -28.0,  0.0,    //  9 - walk left (right foot)
   
   24,   31,   true,   0.0,  0.0,    // 10 - get down (facing right)
   32,   45,   true, 106.0,  0.0,    // 11 - roll
   46,   49,   true,  20.0,  0.0,    // 12 - get up 

   24,   31,  false,   0.0,  0.0,    // 13 - get down (facing left)
   32,   45,  false,-106.0,  0.0,    // 14 - roll
   46,   49,  false, -20.0,  0.0,    // 15 - get up 

   50,   66,   true,   0.0,  0.0,    // 16 - jump up (facing right)
   67,   78,   true,   0.0,  0.0,    // 17 - coming back down (facing right)

   50,   66,  false,   0.0,  0.0,    // 16 - jump up (facing right)
   67,   78,  false,   0.0,  0.0,    // 17 - coming back down (facing right)
   
};

double    lastAnimSecs = 0.0;
GLint     currentAnim = CR_ANIM_LOOK_LEFT;
GLint     currentSprite = 12;

//////////////////////////////////////////////////////////
// error handling

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
void engineSetErrorCallback(EngineError pCallback) {
  engineErrCallback = pCallback;
  
  // Set our callbacks in our support libraries
  shaderSetErrorCallback(engineErrCallback);
  tsSetErrorCallback(engineErrCallback);
  spSetErrorCallback(engineErrCallback);
};

//////////////////////////////////////////////////////////
// fonts
void load_font() {
  // we start with creating a font context that tells us about the font we'll be rendering
  fs = gl3fonsCreate(512, 512, FONS_ZERO_TOPLEFT);
  if (fs != NULL) {
    // then we load our font
    font = fonsAddFont(fs, "sans", "DroidSerif-Regular.ttf");
    if (font != FONS_INVALID) {
      // setup our font
      fonsSetColor(fs, gl3fonsRGBA(255,255,255,255)); // white
      fonsSetSize(fs, 32.0f); // 32 point font
      fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP); // left/top aligned
      fonsVertMetrics(fs, NULL, NULL, &lineHeight);
    } else {
      engineErrCallback(-201, "Couldn't load DroidSerif-Regular.ttf");       
    };
  } else {
    engineErrCallback(-200, "Couldn't create our font context");
  };
};

void unload_font() {
  if (fs != NULL) {
    gl3fonsDelete(fs);
    fs = NULL;
  };
};

//////////////////////////////////////////////////////////
// Shaders

// load contents of a file
// return NULL on failure
// returns string on success, calling function is responsible for freeing the text
char* loadFile(const char* pFileName) {
  char  error[1024];
  char* result = NULL;

  // read "binary", we simply keep our newlines as is
  FILE* file = fopen(pFileName,"rb");
  if (file == NULL) {
    sprintf(error,"Couldn't open %s",pFileName);
    engineErrCallback(-100, error);
  } else {
    long size;
    
    // find the end of the file to get its size
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // allocate space for our data
    result = (char*) malloc(size+1);
    if (result == NULL) {
      sprintf(error,"Couldn't allocate memory buffer for %li bytes",size);
      engineErrCallback(-101, error);    
    } else {
      // and load our text
      fread(result, 1, size, file);
      result[size] = 0;
    };
    
    fclose(file);
  };
  
  return result;
};

void load_shaders() {
  // load our tilemap shader
  tsSetLoadFileFunc(loadFile);
  tsLoad(&ts);
  
  // load our spritesheet shader
  spSetLoadFileFunc(loadFile);
  spLoad(&sp);
};

void unload_shaders() {
  tsUnload(&ts);
  spUnload(&sp);
};

//////////////////////////////////////////////////////////
// Objects

void setTexture(GLuint pTexture, GLint pFilter, GLint pWrap) {
  glBindTexture(GL_TEXTURE_2D, pTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, pWrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, pWrap);  
};

void load_objects() {
	int x, y, comp;
  unsigned char * data;

  // we start with creating our vertex array object, even though we're not doing much with it, OpenGL still requires one for state
  glGenVertexArrays(1, &VAO);
  
  // Need to create our texture objects
  glGenTextures(TEXT_COUNT, textures);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////
  // now load in our map texture into textures[TEXT_MAPDATA]
	setTexture(textures[TEXT_MAPDATA], GL_LINEAR, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 40, 40, 0, GL_RED, GL_UNSIGNED_BYTE, mapdata);
  ts.mapdataTexture = textures[TEXT_MAPDATA];
  
  // and we load our tiled map into textures[TEXT_TILEDATA]
	data = stbi_load("desert256.png", &x, &y, &comp, 4);
  if (data == 0) {
    engineErrCallback(-1, "Couldn't load desert256.png");
  } else {
  	setTexture(textures[TEXT_TILEDATA], GL_LINEAR, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    ts.tileTexture = textures[TEXT_TILEDATA];
		
		stbi_image_free(data);
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // now load in our sprite texture into textures[TEXT_SPRITEDATA]
	data = stbi_load("conrad.png", &x, &y, &comp, 4);
  if (data == 0) {
    engineErrCallback(-1, "Couldn't load conrad.png");
  } else {
  	setTexture(textures[TEXT_SPRITEDATA], GL_LINEAR, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    sp.spriteTexture = textures[TEXT_SPRITEDATA];
    sp.spriteWidth = x;
    sp.spriteHeight = y;
		
		stbi_image_free(data);
  };

  // normally we would load this from a file or another source
  // copying from a memory buffer like this is a bit wasteful.
  spAddSprites(&sp, conradSpriteSheet, sizeof(conradSpriteSheet) / sizeof(sprite));

  // and lets be nice and unbind...
	glBindTexture(GL_TEXTURE_2D, 0);

  // and clear our selected vertex array object
  glBindVertexArray(0);
};

void unload_objects() {
  sp.spriteTexture = 0;
  ts.tileTexture = 0;
  ts.mapdataTexture = 0;
  
  glDeleteTextures(TEXT_COUNT, textures);
  glDeleteVertexArrays(1, &VAO);
};

//////////////////////////////////////////////////////////
// Main engine

// engineLoad loads any data that we need to load before we can start outputting stuff
void engineLoad() {
  // load our font
  load_font();
    
  // load, compile and link our shader(s)
  load_shaders();
  
  // load our objects
  load_objects();
  
  // init our view matrix (for now just an identity matrix)
  mat4Identity(&view);  
};

// engineUnload unloads and frees up any data associated with our engine
void engineUnload() {
  // lets be nice and cleanup
  unload_shaders();
  unload_objects();
  unload_font();
};

// engineUpdate is called to handle any updates of our data
// pSecondsPassed is the number of seconds passed since our application was started.
void engineUpdate(double pSecondsPassed) {
  double delta;
  bool   animReset = false;
  
  // update our animation
  delta = (pSecondsPassed - lastAnimSecs);
  if (delta > (1.0 / 10.0)) { // roughly aim for 10 frames per second
    lastAnimSecs = pSecondsPassed;
    
    if (conradAnim[currentAnim].lastSprite < conradAnim[currentAnim].firstSprite) {
      // move backwards one sprite
      currentSprite--;
      if (currentSprite < conradAnim[currentAnim].lastSprite) {
        currentSprite = conradAnim[currentAnim].firstSprite;
        animReset = true;
      };
    } else {      
      // move forewards one sprite
      currentSprite++;
      if (currentSprite > conradAnim[currentAnim].lastSprite) {
        currentSprite = conradAnim[currentAnim].firstSprite;
        animReset = true;
      };
    };
    
    // if our animation was reset we have a chance to change the animation
    if (animReset) {
      int key = popKey(); // pop a key, 0 = no key on stack
      if (key == GLFW_KEY_EQUAL) {
        currentAnim++;
        if (currentAnim == CR_ANIM_COUNT) {
          currentAnim = 0;
        };
        currentSprite = conradAnim[currentAnim].firstSprite;        
      } else if (key == GLFW_KEY_MINUS) {
        currentAnim--;
        if (currentAnim < 0) {
          currentAnim = CR_ANIM_COUNT - 1;
        };
        currentSprite = conradAnim[currentAnim].firstSprite;        
      };
    };
  };
  
  // update our frame counter
  frames += 1.0f;
  delta = pSecondsPassed - lastsecs;
  fps = (frames - lastframes) / delta;

  if (frames - lastframes > 100.0f) {
    // reset every 100 frames
    lastsecs = pSecondsPassed;
    lastframes = frames;
  };
};

// engineRender is called to render our stuff
void engineRender(int pWidth, int pHeight) {;
  mat4 mvp, invmvp, projection, modelview;
  vec3 tmpvector;
  float ratio, top;
  int a, b, i;
  
  // select our default VAO so we can render stuff that doesn't need our VAO
  glBindVertexArray(VAO);
  
  // enable our depth test, with this disabled our second triangle paints over our first..
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // set our model view projection matrix
  ratio = pWidth / (float) pHeight;

  // init our projection matrix, note that we've got positive Y pointing down
  mat4Identity(&projection);
  mat4Ortho(&projection, -ratio * 500.0, ratio * 500.0, 500.0f, -500.0f, 1.0f, -1.0f);
          
  // draw our tiled background
//  tsRender(&ts, &projection, &view);
  
  // draw our character
  mat4Copy(&modelview, &view);
  sp.spriteScale = 3.0;
  spRender(&sp, &projection, &modelview, currentSprite, conradAnim[currentAnim].flip, false);
  
  // Testing alignment of our sprites
  sp.spriteScale = 4.0;
  a = conradAnim[currentAnim].firstSprite;
  b = conradAnim[currentAnim].lastSprite;
  if (a > b) {
    int c;
    c = a;
    a = b;
    b = c;
  };
  top = -150.0;
  
  for (i = a; i <= b; i++) {
    mat4Copy(&modelview, &view);
    mat4Translate(&modelview, vec3Set(&tmpvector, 300.0, top, i * -0.001));
    spRender(&sp, &projection, &modelview, i, conradAnim[currentAnim].flip, false);

    mat4Copy(&modelview, &view);
    mat4Translate(&modelview, vec3Set(&tmpvector, -300.0, top, i * 0.001));
    spRender(&sp, &projection, &modelview, i, conradAnim[currentAnim].flip, false);
    
    top += 40.0;
  };
  
  // unset stuff
  glBindVertexArray(0);
  glUseProgram(0);
  
  ////////////////////////////////////
  // UI
    
  // change our state a little
  glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  
  
  // now render our FPS
	if ((fs != NULL) && (font != FONS_INVALID)) {
    char info[256];
    
    // we can use our same projection matrix
    gl3fonsProjection(fs, (GLfloat *)projection.m);

    // what text shall we draw?
    sprintf(info,"FPS: %0.1f, anim = %i, index = %i", fps, currentAnim, currentSprite);
        
    // and draw some text
    fonsDrawText(fs, -ratio * 500.0f, 460.0f, info, NULL);
  };
};

void engineKeyPressed(int pKey) {
  pushKey(pKey);
};


