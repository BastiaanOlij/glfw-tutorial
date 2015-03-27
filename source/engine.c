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
GLuint VAO = 0;
GLuint VBOs[2] = { 0, 0 };

// and some globals for our fonts
FONScontext *	fs = NULL;
int font = FONS_INVALID;
float lineHeight = 0.0f;

// and some runtime variables.
double rotation = 0.0f;
double frames = 0.0f;
double fps = 0.0f;
double lastframes = 0.0f;
double lastsecs = 0.0f;

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
          shaderProgram = shaderLink(2, vertexShader, fragmentShader);
          mvpId = glGetUniformLocation(shaderProgram, "mvp");
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
  if (shaderProgram != NO_SHADER) {
    glDeleteProgram(shaderProgram);
    shaderProgram = NO_SHADER;
  };
};

//////////////////////////////////////////////////////////
// Objects

void load_objects() {
  // data for our triangle
  GLfloat vertices[9] = {
    -300.0f, -200.0f,    0.0f,
     300.0f, -200.0f,    0.0f,
       0.0f,  300.0f,    0.0f
  };
  GLfloat colors[9] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };
  GLuint indices[3] = { 0, 1, 2 };
    
  // we start with creating our vertex array object
  glGenVertexArrays(1, &VAO);
  
  // and make it current, all actions we do now relate to this VAO
  glBindVertexArray(VAO);
  
  // and create our two vertex buffer objects
  glGenBuffers(2, VBOs);
  
  // load up our vertices
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  
  // size our buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
  
  // layout (location=0) in vec3	vertices;
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid *) 0);
  
  // layout (location=1) in vec3	colors;
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid *) sizeof(vertices));
  
  // load up our indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  // and clear our selected vertex array object
  glBindVertexArray(0);
};

void unload_objects() {
  glDeleteBuffers(2, VBOs);
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
  mat4 mvp, projection, view;
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
  
  // init our view matrix (for now just an identity matrix)
  mat4Identity(&view);
        
  // select our shader
  if ((shaderProgram != NO_SHADER) && (mvpId >= 0)) {
    glUseProgram(shaderProgram);

    // draw our triangle 1x
    mat4Copy(&mvp, &projection);
    mat4Multiply(&mvp, &view);
    mat4Rotate(&mvp, (float) rotation, vec3Set(&tmpvector, 0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(mvpId, 1, false, (const GLfloat *) mvp.m);
  
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

    // draw our triangle 2x
    mat4Copy(&mvp, &projection);
    mat4Multiply(&mvp, &view);
    mat4Translate(&mvp, vec3Set(&tmpvector, -200.0f, -100.0f, 0.5f)); // move a little to the left, a little up, and a little back
    mat4Rotate(&mvp, (float) -rotation, vec3Set(&tmpvector, 0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(mvpId, 1, false, (const GLfloat *) mvp.m);
  
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
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
