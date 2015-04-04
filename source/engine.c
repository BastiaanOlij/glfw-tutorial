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
GLuint shaderProgram = NO_SHADER;
GLint  mvpId = -1;
GLint  mapdataId = -1;
GLint  tileId = -1;
GLuint VAO = 0;
GLuint textures[2] = { 0, 0 };

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

//////////////////////////////////////////////////////////
// error handling
EngineError engineErrCallback = NULL;

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
void engineSetErrorCallback(EngineError pCallback) {
  engineErrCallback = pCallback;
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
  char* shaderText = NULL;
  GLuint vertexShader = NO_SHADER, fragmentShader = NO_SHADER;
  
  // set our error callback...
  shaderSetErrorCallback(engineErrCallback);
  
  // all our error reporting is already done in these functions
  // so we can keep going as long as we have no error
  // and cleanup after ourselves...
  shaderText = loadFile("tilemap.vs");
  if (shaderText != NULL) {
    vertexShader = shaderCompile(GL_VERTEX_SHADER, shaderText);
    free(shaderText);
    
    if (vertexShader != NO_SHADER) {
      shaderText = loadFile("tilemap.fs");

      if (shaderText != NULL) {
        fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, shaderText);
        free(shaderText);
        
        if (fragmentShader != NO_SHADER) {
          shaderProgram = shaderLink(2, vertexShader, fragmentShader);
          mvpId = glGetUniformLocation(shaderProgram, "mvp");
          if (mvpId < 0) {
            engineErrCallback(mvpId, "Unknown uniform mvp");
          };
          mapdataId = glGetUniformLocation(shaderProgram, "mapdata");
          if (mapdataId < 0) {
            engineErrCallback(mapdataId, "Unknown uniform mapdata");
          };
          tileId = glGetUniformLocation(shaderProgram, "tiles");
          if (tileId < 0) {
            engineErrCallback(tileId, "Unknown uniform tiles");
          };
        };
                
        // no longer need this...
        glDeleteShader(fragmentShader);
      };
      
      // no longer need this...
      glDeleteShader(vertexShader);
    };
  };
};

void unload_shaders() {
  if (shaderProgram != NO_SHADER) {
    glDeleteProgram(shaderProgram);
    shaderProgram = NO_SHADER;
  };
};

//////////////////////////////////////////////////////////
// Objects

void load_objects() {
	int x, y, comp;
  unsigned char * data;

  // we start with creating our vertex array object, even though we're not doing much with it, OpenGL still requires one for state
  glGenVertexArrays(1, &VAO);
  
  // Need to create our texture objects
  glGenTextures(2, textures);
  
  // now load in our map texture into textures[0]
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 40, 40, 0, GL_RED, GL_UNSIGNED_BYTE, mapdata);
  
  // and we load our tiled map
	data = stbi_load("desert256.png", &x, &y, &comp, 4);
  if (data == 0) {
    engineErrCallback(-1, "Couldn't load desert256.png");
  } else {
  	glBindTexture(GL_TEXTURE_2D, textures[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		stbi_image_free(data);
  };

  // and lets be nice and unbind...
	glBindTexture(GL_TEXTURE_2D, 0);

  // and clear our selected vertex array object
  glBindVertexArray(0);
};

void unload_objects() {
  glDeleteTextures(2, textures);
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
  mat4 mvp, projection;
  vec3 tmpvector;
  float ratio;
  
  // enable our depth test, with this disabled our second triangle paints over our first..
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // set our model view projection matrix
  ratio = pWidth / (float) pHeight;

  // init our projection matrix, note that we've got positive Y pointing down
  mat4Identity(&projection);
  mat4Ortho(&projection, -ratio * 500.0, ratio * 500.0, 500.0f, -500.0f, 1.0f, -1.0f);
          
  // select our shader
  if ((shaderProgram != NO_SHADER) && (mvpId >= 0)) {
    glUseProgram(shaderProgram);

    // draw our map
    
    // set our model-view-projection matrix first
    mat4Copy(&mvp, &projection);
    mat4Multiply(&mvp, &view);
    glUniformMatrix4fv(mvpId, 1, false, (const GLfloat *) mvp.m);
    
    // now tell it which textures to use
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glUniform1i(mapdataId, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glUniform1i(tileId, 1);
    
    // and draw our triangles
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 40 * 40 * 3 * 2);
    glBindVertexArray(0);
  
    // unset our shader
    glUseProgram(0);
  };
  
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
    sprintf(info,"FPS: %0.1f", fps);
        
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
