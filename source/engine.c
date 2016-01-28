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

// object info
llist *       materials = NULL;
llist *       meshes = NULL;

// and some globals for our fonts
FONScontext * fs = NULL;
int           font = FONS_INVALID;
float         lineHeight = 0.0f;

// shaders
shaderStdInfo   colorShader;
shaderStdInfo   texturedShader;
shaderStdInfo   reflectShader;

// lights
lightSource   sun;

// our camera
mat4          view;
vec3          camera_eye = { -200.0, 20.0, 500.0 };
vec3          camera_lookat =  { -300.0, 100.0, -50.0 };

// and some runtime variables.
double        frames = 0.0f;
double        fps = 0.0f;
double        lastframes = 0.0f;
double        lastsecs = 0.0f;


//////////////////////////////////////////////////////////
// error handling

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
void engineSetErrorCallback(EngineError pCallback) {
  engineErrCallback = pCallback;
  
  // Set our callbacks in our support libraries
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

  shaderText = loadFile("standard.vs");
  if (shaderText != NULL) {
    vertexShader = shaderCompile(GL_VERTEX_SHADER, shaderText);
    free(shaderText);
    
    if (vertexShader != NO_SHADER) {
      // compile our color shader
      shaderText = loadFile("flatcolor.fs");
      if (shaderText != NULL) {
        fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, shaderText);
        free(shaderText);
        
        if (fragmentShader != NO_SHADER) {
          colorShader = shaderGetStdInfo(shaderLink(2, vertexShader, fragmentShader));

          // no longer need this...
          glDeleteShader(fragmentShader);
        };                
      };

      // compile our textured shader
      shaderText = loadFile("textured.fs");
      if (shaderText != NULL) {
        fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, shaderText);
        free(shaderText);
        
        if (fragmentShader != NO_SHADER) {
          texturedShader = shaderGetStdInfo(shaderLink(2, vertexShader, fragmentShader));

          // no longer need this...
          glDeleteShader(fragmentShader);
        };                
      };

      // compile our refleect shader
      shaderText = loadFile("reflect.fs");
      if (shaderText != NULL) {
        fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, shaderText);
        free(shaderText);
        
        if (fragmentShader != NO_SHADER) {
          reflectShader = shaderGetStdInfo(shaderLink(2, vertexShader, fragmentShader));

          // no longer need this...
          glDeleteShader(fragmentShader);
        };                
      };
      
      // no longer need this...
      glDeleteShader(vertexShader);
    };
  };
};

void unload_shaders() {
  // should add proper functions for freeing up shaders here
  glDeleteProgram(colorShader.program);
  glDeleteProgram(texturedShader.program);
  glDeleteProgram(reflectShader.program);
};

//////////////////////////////////////////////////////////
// Objects

void load_objects() {
	int x, y, comp;
  unsigned char * data;
  char * text;
  vec3 tmpvector;
  mesh3d * mesh;
  material * mat;
  
  // create a retainer for materials
  materials = newMatList();
 
  // create a retainer for meshes
  meshes = newMeshList();
  if (meshes != NULL) {
    llistNode * node;

/*    
    mat = newMaterial("cube");                  // create a material for our cube
    matLoadTexture(mat, "boxtexture.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE);  // load our texture map
    mesh = newMesh(24, 36);                     // init our cube with enough space for our buffers
    strcpy(mesh->name,"cube");                  // set name to cube
    meshSetMaterial(mesh, mat);                 // assign our material
    matRelease(mat);                            // and release it, our mesh now owns it
    meshMakeCube(mesh, 10.0, 10.0, 10.0);       // create our cube
    mat4Translate(&mesh->defModel, vec3Set(&tmpvector, -10.0, 0.0, 0.0)); // position it
    meshCopyToGL(mesh, true);                   // copy our cube data to the GPU
    llistAddTo(meshes, mesh);                   // add it to our list
    meshRelease(mesh);                          // and release it, our list now owns it
*/
    
    mat = newMaterial("sphere");                // create a material for our sphere
    matLoadTexture(mat, "EarthMap.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE);  // load our texture map
    mesh = newMesh(10585, 62208);               // init our sphere with default space for our buffers
    strcpy(mesh->name,"sphere");                // set name to sphere
    meshSetMaterial(mesh, mat);                 // assign our material
    matRelease(mat);                            // and release it, our mesh now owns it
    meshMakeSphere(mesh, 100.0);                // create our sphere
    mat4Translate(&mesh->defModel, vec3Set(&tmpvector, 100.0, 0.0, 0.0)); // position it
    meshCopyToGL(mesh, true);                   // copy our sphere data to the GPU
    llistAddTo(meshes, mesh);                   // add it to our list
    meshRelease(mesh);                          // and release it, our list now owns it

    // load our mtl file
    text = loadFile("tie-bomber.mtl");
    if (text != NULL) {
      matParseMtl(text, materials);
      
      free(text);
    };    

    // load our obj file
    text = loadFile("tie-bomber.obj");
    if (text != NULL) {
      meshParseObj(text, meshes, materials, true);
      
      free(text);
    };    
  };
};

void unload_objects() {
  // free our mesh list, this will also destoy all the meshes within
  if (meshes != NULL) {
    llistFree(meshes);
    meshes = NULL;
  };
  
  if (materials != NULL) {
    llistFree(materials);
    materials = NULL;
  };
};

//////////////////////////////////////////////////////////
// Main engine

// engineInit initializes any variables, kinda like our constructor
void engineInit() {

};

// engineLoad loads any data that we need to load before we can start outputting stuff
void engineLoad() {
  vec3  upvector;
  
  // load our font
  load_font();
    
  // load, compile and link our shader(s)
  load_shaders();
  
  // setup our light
  vec3Set(&sun.position, 0.0, 10000.0, 5000.00);
  
  // load our objects
  load_objects();
  
  // init our view matrix
  mat4Identity(&view);
  mat4LookAt(&view, &camera_eye, &camera_lookat, vec3Set(&upvector, 0.0, 1.0, 0.0));  
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
  vec3  avector, bvector, upvector;
  mat4  M;
    
  // handle our keys....
  if (engineKeyPressedCallback(GLFW_KEY_A)) {
    // rotate position left
    
    // get our (reverse) looking direction vector
    vec3Copy(&avector, &camera_eye);
    vec3Sub(&avector, &camera_lookat);
    
    // rotate our looking direction vector around our up vector
    mat4Identity(&M);
    mat4Rotate(&M, 1.0, vec3Set(&bvector, view.m[0][1], view.m[1][1], view.m[2][1]));
    
    // and update our eye position accordingly
    mat4ApplyToVec3(&camera_eye, &avector, &M);
    vec3Add(&camera_eye, &camera_lookat);
  } else if (engineKeyPressedCallback(GLFW_KEY_D)) {
    // rotate position right
    
    // get our (reverse) looking direction vector
    vec3Copy(&avector, &camera_eye);
    vec3Sub(&avector, &camera_lookat);
    
    // rotate our looking direction vector around our up vector
    mat4Identity(&M);
    mat4Rotate(&M, -1.0, vec3Set(&bvector, view.m[0][1], view.m[1][1], view.m[2][1]));
    
    // and update our eye position accordingly
    mat4ApplyToVec3(&camera_eye, &avector, &M);
    vec3Add(&camera_eye, &camera_lookat);
  } else if (engineKeyPressedCallback(GLFW_KEY_W)) {
    // get our (reverse) looking direction vector
    vec3Copy(&avector, &camera_eye);
    vec3Sub(&avector, &camera_lookat);
    
    // rotate our looking direction vector around our right vector
    mat4Identity(&M);
    mat4Rotate(&M, 1.0, vec3Set(&bvector, view.m[0][0], view.m[1][0], view.m[2][0]));
    
    // and update our eye position accordingly
    mat4ApplyToVec3(&camera_eye, &avector, &M);
    vec3Add(&camera_eye, &camera_lookat);    
  } else if (engineKeyPressedCallback(GLFW_KEY_S)) {
    // get our (reverse) looking direction vector
    vec3Copy(&avector, &camera_eye);
    vec3Sub(&avector, &camera_lookat);
    
    // rotate our looking direction vector around our right vector
    mat4Identity(&M);
    mat4Rotate(&M, -1.0, vec3Set(&bvector, view.m[0][0], view.m[1][0], view.m[2][0]));
    
    // and update our eye position accordingly
    mat4ApplyToVec3(&camera_eye, &avector, &M);
    vec3Add(&camera_eye, &camera_lookat);    
  };

  // update our view matrix
  mat4Identity(&view);
  mat4LookAt(&view, &camera_eye, &camera_lookat, vec3Set(&upvector, 0.0, 1.0, 0.0));
    
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

// structure for storing info about transparent meshes that we render later on
typedef struct alphaMesh {
  mesh3d *  mesh;
  mat4      model;
  GLfloat   z;
} alphaMesh;

// engineRender is called to render our stuff
// pWidth and pHeight define the size of our drawing buffer
// pMode is 0 => mono, 1 => left eye, 2 => right eye
void engineRender(int pWidth, int pHeight, float pRatio, int pMode) {
  shaderMatrices  matrices;
  vec3            tmpvector;
  float           left, top;
  int             i;
  dynarray *      meshesWithAlpha = newDynArray(sizeof(alphaMesh));

  // calculate our sun position, we want to do this only once
  mat4ApplyToVec3(&sun.adjPosition,&sun.position, &view);
        
  // enable and configure our backface culling
  glEnable(GL_CULL_FACE);   // enable culling
  glFrontFace(GL_CW);       // clockwise
  glCullFace(GL_BACK);      // backface culling
  // enable our depth test
  glEnable(GL_DEPTH_TEST);
  // disable alpha blending  
  glDisable(GL_BLEND);

  // set our model view projection matrix

  // init our projection matrix, we use a 3D projection matrix now
  mat4Identity(&matrices.projection);
//  mat4Projection(&matrices.projection, 45.0, pRatio, 1.0, 10000.0);
  // distance between eyes is on average 6.5 cm, this should be setable
  mat4Stereo(&matrices.projection, 45.0, pRatio, 1.0, 10000.0, 6.5, 200.0, pMode);
  
  // copy our view matrix into our state
  mat4Copy(&matrices.view, &view);
  
  if (meshes != NULL) {
    int count = 0;
    llistNode * node = meshes->first;
    while (node != NULL) {
      mesh3d * mesh = (mesh3d *) node->data;
      if (mesh == NULL) {
        // skip
      } else if (!mesh->visible) {
        // skip
      } else {
        bool doRender = true;
        // set our model matrix, this calculation will be more complex in due time
        mat4Copy(&matrices.model, &mesh->defModel);

        // select our shader, for now we have 3 shaders that do different types of shading
        // eventually we'll add something that allows us to make combinations
        if (mesh->material == NULL) {
          shaderSelectProgram(&colorShader, &matrices, &sun, NULL);
        } else if (mesh->material->alpha != 1.0) {
          // postpone
          alphaMesh aMesh;
          GLfloat z = 0; // we should calculate this by taking our modelview matrix and taking our Z offset
        
          // copy our mesh info
          aMesh.mesh = mesh;
          mat4Copy(&aMesh.model, &matrices.model);
          aMesh.z = z;
        
          // and push it...
          dynArrayPush(meshesWithAlpha, &aMesh); // this copies our structure
          doRender = false; // we're postponing...
        } else if (mesh->material->reflectId != MAT_NO_TEXTURE) {          
          shaderSelectProgram(&reflectShader, &matrices, &sun, mesh->material);          
        } else if (mesh->material->textureId != MAT_NO_TEXTURE) {          
          shaderSelectProgram(&texturedShader, &matrices, &sun, mesh->material);          
        } else {
          shaderSelectProgram(&colorShader, &matrices, &sun, mesh->material);
        };

        if (doRender) {
          // and render it, if we fail we won't render it again
          mesh->visible = meshRender(mesh);          
        };
      };
    
      node = node->next;
      count++;
    };
  };
  
  // now render our alpha meshes
  if (meshesWithAlpha->numEntries > 0) {
    // we should sort meshesWithAlpha by Z
  
    // we blend our colors here...
  	glEnable(GL_BLEND);
  	glBlendEquation(GL_FUNC_ADD);
  	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (i = 0; i < meshesWithAlpha->numEntries; i++) {
      alphaMesh * aMesh = dynArrayDataAtIndex(meshesWithAlpha, i);
    
      mat4Copy(&matrices.model, &aMesh->model);
      if (aMesh->mesh->material->reflectId != MAT_NO_TEXTURE) {          
        shaderSelectProgram(&reflectShader, &matrices, &sun, aMesh->mesh->material);          
      } else if (aMesh->mesh->material->textureId != MAT_NO_TEXTURE) {          
        shaderSelectProgram(&texturedShader, &matrices, &sun, aMesh->mesh->material);          
      } else {
        shaderSelectProgram(&colorShader, &matrices, &sun, aMesh->mesh->material);
      };

      aMesh->mesh->visible = meshRender(aMesh->mesh);
    };    
  };
  
  // and free
  dynArrayFree(meshesWithAlpha);
      
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
    mat4Identity(&matrices.projection);
    mat4Ortho(&matrices.projection, -pRatio * virtualScreenHeight, pRatio * virtualScreenHeight, virtualScreenHeight, -virtualScreenHeight, 1.0f, -1.0f);
    gl3fonsProjection(fs, (GLfloat *)matrices.projection.m);

    // what text shall we draw?
    sprintf(info,"FPS: %0.1f, use wasd to move the camera", fps);
        
    // and draw some text
    fonsDrawText(fs, -pRatio * 250.0f, 230.0f, info, NULL);
  };
};

void engineKeyPressed(int pKey) {
//  pushKey(pKey);
};
