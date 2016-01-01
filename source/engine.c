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
EngineKeyPressed engineKeyPressedCallback = NULL;

GLuint      VAO = 0;
GLuint      VBOs[2] = { 0, 0 };

// and some globals for our fonts
FONScontext *	fs = NULL;
int         font = FONS_INVALID;
float       lineHeight = 0.0f;

// shader
GLuint      program;
GLint       mvpId;

// our view matrix
mat4        view;

// and some runtime variables.
double      frames = 0.0f;
double      fps = 0.0f;
double      lastframes = 0.0f;
double      lastsecs = 0.0f;
double      rotate = 0.0;

//////////////////////////////////////////////////////////
// error handling

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
void engineSetErrorCallback(EngineError pCallback) {
  engineErrCallback = pCallback;
  
  // Set our callbacks in our support libraries
  shaderSetErrorCallback(engineErrCallback);
//  tsSetErrorCallback(engineErrCallback);
//  spSetErrorCallback(engineErrCallback);
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
  char* shaderText = NULL;
  GLuint vertexShader = NO_SHADER, fragmentShader = NO_SHADER;

  shaderText = loadFile("simple.vs");
  if (shaderText != NULL) {
    vertexShader = shaderCompile(GL_VERTEX_SHADER, shaderText);
    free(shaderText);
    
    if (vertexShader != NO_SHADER) {
      shaderText = loadFile("simple.fs");

      if (shaderText != NULL) {
        fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, shaderText);
        free(shaderText);
        
        if (fragmentShader != NO_SHADER) {
          program = shaderLink(2, vertexShader, fragmentShader);
          mvpId = glGetUniformLocation(program, "mvp");
          if (mvpId < 0) {
            engineErrCallback(mvpId, "Unknown uniform mvp");
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
  glDeleteProgram(program);
};

//////////////////////////////////////////////////////////
// Objects

//  We're building a cube:
//
//      5------------4
//     /|           /|
//    / |          / |
//   0------------1  |
//   |  |         |  |
//   |  |         |  |
//   |  6---------|--7
//   | /          | /
//   |/           |/
//   3------------2

// we define a structure for our vertices, for now we define the location and color of each of our vertices on our cube
typedef struct vertex {
  vec3    V;          // position of our vertice (XYZ)
  vec3    C;          // color of our vertice (RGB)
} vertex;

vertex vertices[] = {
  -0.5,  0.5,  0.5, 1.0, 0.0, 0.0,          // vertex 0
   0.5,  0.5,  0.5, 0.0, 1.0, 0.0,          // vertex 1
   0.5, -0.5,  0.5, 0.0, 0.0, 1.0,          // vertex 2
  -0.5, -0.5,  0.5, 1.0, 0.0, 1.0,          // vertex 3
   0.5,  0.5, -0.5, 1.0, 1.0, 0.0,          // vertex 4
  -0.5,  0.5, -0.5, 0.0, 1.0, 1.0,          // vertex 5
  -0.5, -0.5, -0.5, 1.0, 1.0, 1.0,          // vertex 6
   0.5, -0.5, -0.5, 0.0, 0.0, 0.0,          // vertex 7
};

// and now define our indices that make up our triangles
GLint indices[] = {
  0, 1, 2,
  0, 2, 3,

  1, 4, 7,
  1, 7, 2,

  5, 4, 1,
  5, 1, 0,

  3, 2, 7,
  3, 7, 6, 
  
  5, 0, 3,
  5, 3, 6, 
  
  4, 5, 6, 
  4, 6, 7,  
};

void load_objects() {
  // we start with creating our vertex array object
  glGenVertexArrays(1, &VAO);
  glGenBuffers(2, VBOs);
  
  // select our VAO
  glBindVertexArray(VAO);
  
  // now load our vertices into our first VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
  
  // now we need to configure our attributes, we use one for our position and one for our color attribute 
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) sizeof(vec3));
  
  // now we load our indices into our second VBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
  
  // at this point in time our two buffers are bound to our vertex array so any time we bind our vertex array
  // our two buffers are bound aswell

  // and clear our selected vertex array object
  glBindVertexArray(0);
};

void unload_objects() {
  glDeleteBuffers(2, VBOs);
  glDeleteVertexArrays(1, &VAO);
};

//////////////////////////////////////////////////////////
// Main engine

// engineInit initializes any variables, kinda like our constructor
void engineInit() {

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
  
  // init our view matrix, camera looking straight ahead
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
  float delta;
  
  // rotate for display
  rotate = pSecondsPassed * 20.0;
  
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
  mat4 projection, mvp;
  vec3 tmpvector;
  float ratio, left, top;
  
  // select our default VAO so we can render stuff that doesn't need our VAO
  glBindVertexArray(VAO);
  
  // enable and configure our backface culling
	glEnable(GL_CULL_FACE); // enable culling
  glFrontFace(GL_CW);     // clockwise
	glCullFace(GL_BACK);    // backface culling
  // enable our depth test
  glDisable(GL_BLEND);

  // set our model view projection matrix
  ratio = (float) pWidth / (float) pHeight;

  // init our projection matrix, we use a 3D projection matrix now
  mat4Identity(&projection);
  mat4Projection(&projection, 45.0, ratio, 1.0, 10000.0);

  // set our model view projection matrix
  mat4Copy(&mvp, &projection);
  mat4Multiply(&mvp, &view);
  mat4Translate(&mvp, vec3Set(&tmpvector, 0.0, 0.0, -30.0));   // move it back so we can see it
  mat4Rotate(&mvp, rotate, vec3Set(&tmpvector, 10.0, 5.0, 15.0)); // rotate our cube
  mat4Scale(&mvp, vec3Set(&tmpvector, 10.0, 10.0, 10.0));   // make our cube 10x10x10 big

  // select our shader
  glUseProgram(program);
  glUniformMatrix4fv(mvpId, 1, false, (const GLfloat *) mvp.m);

  // now render our cube
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);	
    
  // unset stuff
  glBindVertexArray(0);
  glUseProgram(0);
  
  ////////////////////////////////////
  // UI
    
  // change our state a little
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  
  
  // now render our FPS
	if ((fs != NULL) && (font != FONS_INVALID)) {
    float virtualScreenHeight = 250.0;
    char info[256];
    
    // We want a orthographic projection for our frame counter
    mat4Identity(&projection);
    mat4Ortho(&projection, -ratio * virtualScreenHeight, ratio * virtualScreenHeight, virtualScreenHeight, -virtualScreenHeight, 1.0f, -1.0f);
    gl3fonsProjection(fs, (GLfloat *)projection.m);

    // what text shall we draw?
    sprintf(info,"FPS: %0.1f", fps);
        
    // and draw some text
    fonsDrawText(fs, -ratio * 250.0f, 230.0f, info, NULL);
  };
};

void engineKeyPressed(int pKey) {
//  pushKey(pKey);
};
