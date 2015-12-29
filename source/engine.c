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
// Include our game data
#include "gamedata.h"

//////////////////////////////////////////////////////////
// For now we just some global state to make life easy

EngineError engineErrCallback = NULL;
EngineKeyPressed engineKeyPressedCallback = NULL;

// shader program and buffers
tileshader  ts;
spritesheet sp;

GLuint      VAO = 0;
GLuint      textures[TEXT_COUNT] = { 0, 0, 0 };

// and some globals for our fonts
FONScontext *	fs = NULL;
int         font = FONS_INVALID;
float       lineHeight = 0.0f;

// our view matrix
mat4        view;

// and some runtime variables.
double      frames = 0.0f;
double      fps = 0.0f;
double      lastframes = 0.0f;
double      lastsecs = 0.0f;

double      lastAnimSecs = 0.0;
GLint       currentAnim = CR_ANIM_LOOK_LEFT;
GLint       currentSprite = 0;
vec3        currentPos;

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
// keyboard handling

// sets a callback that allows us to test if a key is
// pressed
void engineSetKeyPressedCallback(EngineKeyPressed pCallback) {
  engineKeyPressedCallback = pCallback;
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
      fonsSetSize(fs, 16.0f); // 16 point font
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 40, 20, 0, GL_RED, GL_UNSIGNED_BYTE, mapdata);
  ts.mapdataTexture = textures[TEXT_MAPDATA];
  ts.mapSize.x = 40.0;
  ts.mapSize.y = 20.0;
  ts.mapScale = TILE_SCALE;
  
  // and we load our tiled map into textures[TEXT_TILEDATA]
	data = stbi_load("tiles.png", &x, &y, &comp, 4);
  if (data == 0) {
    engineErrCallback(-1, "Couldn't load tiles.png");
  } else {
  	setTexture(textures[TEXT_TILEDATA], GL_LINEAR, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    ts.tileTexture = textures[TEXT_TILEDATA];
    ts.tilesPerSide = 16;
    ts.textureSize = x; // assume square
		
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
    sp.texture = textures[TEXT_SPRITEDATA];
    sp.textureSize.x = x;
    sp.textureSize.y = y;
		
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
  sp.texture = 0;
  ts.tileTexture = 0;
  ts.mapdataTexture = 0;
  
  glDeleteTextures(TEXT_COUNT, textures);
  glDeleteVertexArrays(1, &VAO);
};

//////////////////////////////////////////////////////////
// Main engine

// engineInit initializes any variables, kinda like our constructor
void engineInit() {
  tsInit(&ts);
  spInit(&sp);
};

// engineLoad loads any data that we need to load before we can start outputting stuff
void engineLoad() {
  vec3  tmpvector;
  
  // load our font
  load_font();
    
  // load, compile and link our shader(s)
  load_shaders();
  
  // load our objects
  load_objects();
  
  // init our view matrix
  mat4Identity(&view);  
  mat4Translate(&view, vec3Set(&tmpvector, 10 * TILE_SCALE, -2.0 * TILE_SCALE, 0.0));
  
  // and init our character position (within our tilemap)
  vec3Set(&currentPos, -18.0, 9.0, 0.0);
};

// engineUnload unloads and frees up any data associated with our engine
void engineUnload() {
  // lets be nice and cleanup
  unload_shaders();
  unload_objects();
  unload_font();
};

// areKeysPressed: Returns true if all keys in our keys array are currently pressed
bool areKeysPressed(int *keys) {
  if (keys != NULL) {
    while (*keys != 0) {
      if (!engineKeyPressedCallback(*keys)) {
        return false;
      };
      
      keys++;
    };
  };
  return true;
};

// isTileType: Returns true if the tile at our x, y position appears in our tiles array
bool isTileType(GLint x, GLint y, unsigned char *tiles) {
  if (x < 0 | x >= 40) {
    return false;
  } else if (y < 0 | y >= 20) {
    return false;
  };
  
  unsigned char tile = interactiondata[y * 40 + x];
    
  while (*tiles != 0) {
    if (*tiles == tile) {
      return true;
    };
    
    tiles++;
  };
  
  return false;
};

// canMoveThere: Returns true if atleast one move_map entry in our array matches
bool canMoveThere(move_map *moveMap) {
  GLint  x, y;
  
  if (moveMap == NULL) {
    return true;
  };
  
  x = (GLint) currentPos.x + 20;
  y = (GLint) currentPos.y + 10;

  while (moveMap->tiles != NULL) {    
    if (!isTileType(x + moveMap->relX, y + moveMap->relY, moveMap->tiles)) {
      return false;
    };
    
    moveMap++;
  };
  
  return true;
};

// obtain our next animation type
GLint getNextAnim(action_map *followUpActions) {
  GLint nextAnim = -1;

  if (followUpActions != NULL) {
    // check our action map
    while ((nextAnim == -1) & (followUpActions->startAnimation != CR_END)) {
      // assume this will be our next animation until proven differently
      nextAnim = followUpActions->startAnimation;
              
      // check if we're missing any keys
      if (!areKeysPressed(followUpActions->keys)) {
        // keep looking...
        nextAnim = -1;
        followUpActions++; // check next
      } else if (!canMoveThere(followUpActions->moveMap)) {
        // keep looking...
        nextAnim = -1;
        followUpActions++; // check next
      };
    };    
  };  
  
  return nextAnim;
};

// engineUpdate is called to handle any updates of our data
// pSecondsPassed is the number of seconds passed since our application was started.
void engineUpdate(double pSecondsPassed) {
  double delta;
  bool   animReset = false;
  
  // update our animation
  delta = (pSecondsPassed - lastAnimSecs);
  if (delta > (1.0 / 12.0)) { // roughly aim for 12 frames per second
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
      int nextAnim = -1;
      
      // if our current animation was played forward, we add our movement once finished
      if (conradAnim[currentAnim].firstSprite<=conradAnim[currentAnim].lastSprite) {
        currentPos.x += conradAnim[currentAnim].moveX * SPRITE_SCALE;
        currentPos.y += conradAnim[currentAnim].moveY * SPRITE_SCALE;        
      };
      
      nextAnim = getNextAnim(conradAnim[currentAnim].followUpActions);

      if (nextAnim == -1) {
        engineErrCallback(0, "Missing animation from %i", currentAnim);
      };
      
      // if -1  We're missing something
      currentAnim = nextAnim == -1 ? 0 : nextAnim;
      currentSprite = conradAnim[currentAnim].firstSprite;
      
      // if our new animation is going to be played in reverse, we add our movement at the start
      if (conradAnim[currentAnim].firstSprite>conradAnim[currentAnim].lastSprite) {
        currentPos.x += conradAnim[currentAnim].moveX * SPRITE_SCALE;
        currentPos.y += conradAnim[currentAnim].moveY * SPRITE_SCALE;        
      };
    };
  };
  
  // update our view
  if ((view.m[3][0] / TILE_SCALE) + currentPos.x > 5.0f) {
    view.m[3][0] -= TILE_SCALE * 10.0;
    if (view.m[3][0] < TILE_SCALE * -10.0) {
      view.m[3][0] = TILE_SCALE * -10.0;
    };
  } else if ((view.m[3][0] / TILE_SCALE) + currentPos.x < -5.0f) {
    view.m[3][0] += TILE_SCALE * 10.0;
    if (view.m[3][0] > TILE_SCALE * 10.0) {
      view.m[3][0] = TILE_SCALE * 10.0;
    };
  };

  if ((view.m[3][1] / TILE_SCALE) + currentPos.y > 2.5f) {
    view.m[3][1] -= TILE_SCALE * 5.0;
    if (view.m[3][1] < TILE_SCALE * -2.0) {
      view.m[3][1] = TILE_SCALE * -2.0;
    };
  } else if ((view.m[3][1] / TILE_SCALE) + currentPos.y < -2.5f) {
    view.m[3][1] += TILE_SCALE * 5.0;
    if (view.m[3][1] > TILE_SCALE * 2.0) {
      view.m[3][1] = TILE_SCALE * 2.0;
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
  float ratio, left, top;
  float virtualScreenHeight = 250.0;
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
  mat4Ortho(&projection, -ratio * virtualScreenHeight, ratio * virtualScreenHeight, virtualScreenHeight, -virtualScreenHeight, 1.0f, -1.0f);
    
  // draw our tiled background
  tsRender(&ts, &projection, &view);
  
  // Convert our tilemap position of our character to our real location
  vec3Copy(&tmpvector, &currentPos);
  vec3Scale(&tmpvector, TILE_SCALE);
  tmpvector.x += SPRITE_X_CENTER;
  tmpvector.y += SPRITE_Y_CENTER;

  // draw our character
  mat4Copy(&modelview, &view);
  mat4Translate(&modelview, &tmpvector);
  sp.spriteScale = SPRITE_SCALE;
  spRender(&sp, &projection, &modelview, currentSprite, conradAnim[currentAnim].flip, false);
  
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
    float  posX, posY, viewX, viewY;
    
    posX = currentPos.x;
    posY = currentPos.y;
    
    viewX = view.m[3][0] / TILE_SCALE;
    viewY = view.m[3][1] / TILE_SCALE;
    
    // we can use our same projection matrix
    gl3fonsProjection(fs, (GLfloat *)projection.m);

    // what text shall we draw?
    sprintf(info,"FPS: %0.1f, anim = %i, index = %i, tile = %.2f, %.2f, view = %.2f, %.2f", fps, currentAnim, currentSprite, posX, posY, viewX, viewY);
        
    // and draw some text
    fonsDrawText(fs, -ratio * 250.0f, 230.0f, info, NULL);
  };
};

void engineKeyPressed(int pKey) {
//  pushKey(pKey);
};


