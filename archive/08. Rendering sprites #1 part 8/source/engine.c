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

// and some runtime variables.
double rotation = 0.0f;
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
     0.0,  0.0, 20.0, 78.0,  //  0 -- turn from looking left
    20.0,  0.0, 26.0, 78.0,  //  1
    46.0,  0.0, 25.0, 78.0,  //  2
    71.0,  0.0, 25.0, 78.0,  //  3
    96.0,  0.0, 27.0, 78.0,  //  4
   123.0,  0.0, 25.0, 78.0,  //  5
   148.0,  0.0, 21.0, 78.0,  //  6 -- to looking right
   169.0,  0.0, 27.0, 78.0,  //  7 -- start walking left
   196.0,  0.0, 26.0, 78.0,  //  8
   222.0,  0.0, 24.0, 78.0,  //  9
   246.0,  0.0, 20.0, 78.0,  // 10
   266.0,  0.0, 26.0, 78.0,  // 11 -- finish start walking left
   292.0,  0.0, 31.0, 78.0,  // 12 -- start walking left
   323.0,  0.0, 45.0, 78.0,  // 13 
   368.0,  0.0, 45.0, 78.0,  // 14
   413.0,  0.0, 39.0, 80.0,  // 15
   452.0,  0.0, 30.0, 80.0,  // 16
   482.0,  0.0, 26.0, 80.0,  // 17
   508.0,  0.0, 38.0, 80.0,  // 18
   546.0,  0.0, 48.0, 80.0,  // 19
   594.0,  0.0, 46.0, 80.0,  // 20
   640.0,  0.0, 36.0, 82.0,  // 21
   676.0,  0.0, 30.0, 82.0,  // 22
   706.0,  0.0, 28.0, 82.0,  // 23 -- finished walking left 
    
// row 2  
     0.0, 96.0, 20.0, 78.0,  // 24 -- get down
    20.0, 96.0, 22.0, 78.0,  // 25
    42.0, 96.0, 26.0, 78.0,  // 26
    68.0, 96.0, 28.0, 78.0,  // 27
    96.0, 96.0, 30.0, 78.0,  // 28
   126.0, 96.0, 30.0, 78.0,  // 29
   156.0, 96.0, 30.0, 78.0,  // 30
   186.0, 96.0, 30.0, 78.0,  // 31 -- finish get down
   216.0, 96.0, 70.0, 78.0,  // 32 -- start roll
   286.0, 96.0, 80.0, 78.0,  // 33
   366.0, 96.0, 70.0, 80.0,  // 34
   436.0, 96.0, 60.0, 80.0,  // 35
   496.0, 96.0, 58.0, 80.0,  // 36
   554.0, 96.0, 50.0, 80.0,  // 37
   604.0, 96.0, 42.0, 80.0,  // 38
   646.0, 96.0, 42.0, 80.0,  // 39
   688.0, 96.0, 44.0, 80.0,  // 40
   732.0, 96.0, 44.0, 80.0,  // 41 --

// row 3  
     0.0, 188.0, 40.0, 76.0,  // 42
    40.0, 188.0, 48.0, 76.0,  // 43
    88.0, 188.0, 48.0, 78.0,  // 44
   136.0, 188.0, 48.0, 80.0,  // 45 -- finish roll
   184.0, 188.0, 46.0, 80.0,  // 46 -- get up
   230.0, 188.0, 36.0, 80.0,  // 47
   266.0, 188.0, 32.0, 80.0,  // 48
   298.0, 188.0, 28.0, 80.0,  // 49 -- finish get up
// need to complete this...

};

//////////////////////////////////////////////////////////
// error handling
EngineError engineErrCallback = NULL;

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
  rotation = pSecondsPassed * 50.0f;
  
  frames += 1.0f;
  fps = (frames - lastframes) / (pSecondsPassed - lastsecs);

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
  float ratio;
  int index;
  
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
  tsRender(&ts, &projection, &view);
  
  // draw our character
  index = (frames / 10.0);
  index = index % sp.spriteCount;
  mat4Copy(&modelview, &view);
  spRender(&sp, &projection, &modelview, index, true);
  
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
    sprintf(info,"FPS: %0.1f, index = %i", fps, index);
        
    // and draw some text
    fonsDrawText(fs, -ratio * 500.0f, 460.0f, info, NULL);
  };
};

void engineViewRotate(float pAngle) {
  vec3 axis;
  mat4 rotate;
  
  mat4Identity(&rotate);
  mat4Rotate(&rotate, pAngle, vec3Set(&axis, 0.0, 0.0, 1.0));
  mat4Multiply(&rotate, &view);
  mat4Copy(&view, &rotate);
};

void engineViewMove(float pX, float pY) {
  vec3 translate;
  
  // we apply our matrix "transposed" to get counter rotate our movement to our rotation
  vec3Set(&translate
    , pX * view.m[0][0] + pY * view.m[0][1]
    , pX * view.m[1][0] + pY * view.m[1][1]
    , 0.0  
  );
  
  mat4Translate(&view, &translate);  
};
