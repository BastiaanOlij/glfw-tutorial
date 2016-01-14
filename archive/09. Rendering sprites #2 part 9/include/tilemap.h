/********************************************************
 * Tilemap contains logic to render tiles with
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define TILEMAP_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. It also requires math3d.h and shaders.h
 * to be included before hand
 *
 ********************************************************/

#ifndef tilemaph
#define tilemaph

#ifdef __cplusplus
extern "C" {
#endif

// structure for containing our tile shader
typedef struct tileshader {
  GLuint program;
  GLint  mvpId;
  GLint  mapdataId;
  GLuint mapdataTexture;
  GLint  tileId;
  GLuint tileTexture;
} tileshader;

typedef void(* TSError)(int, const char*);
void tsSetErrorCallback(TSError pCallback);

typedef char*(* TSloadFile)(const char*);
void tsSetLoadFileFunc(TSloadFile pLoadFile);

void tsLoad(tileshader* pTS);
void tsUnload(tileshader* pTS);
void tsRender(tileshader* pTS, const mat4* pProjection, const mat4* pView);

#define newtileshader(ts) tileshader ts = { NO_SHADER, -1, -1, 0, -1, 0 }

#ifdef __cplusplus
};
#endif

#ifdef TILEMAP_IMPLEMENTATION

//////////////////////////////////////////////////////////
// error handling

TSError tsErrCallback = NULL;

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
void tsSetErrorCallback(TSError pCallback) {
  tsErrCallback = pCallback;
};

//////////////////////////////////////////////////////////
// file handling

TSloadFile tsLoadFile = NULL;

// sets the function used to load our file, this way we 
// can change this to something else
void tsSetLoadFileFunc(TSloadFile pLoadFile) {
  tsLoadFile = pLoadFile;
};

//////////////////////////////////////////////////////////
// tile shaders

// loads, compiles and links our tileshader
void tsLoad(tileshader* pTS) {
  char* shaderText = NULL;
  GLuint vertexShader = NO_SHADER, fragmentShader = NO_SHADER;

  if ((tsErrCallback == NULL) || (tsLoadFile == NULL)) {
    return;
  };
  
  shaderText = tsLoadFile("tilemap.vs");
  if (shaderText != NULL) {
    vertexShader = shaderCompile(GL_VERTEX_SHADER, shaderText);
    free(shaderText);
    
    if (vertexShader != NO_SHADER) {
      shaderText = tsLoadFile("tilemap.fs");

      if (shaderText != NULL) {
        fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, shaderText);
        free(shaderText);
        
        if (fragmentShader != NO_SHADER) {
          pTS->program = shaderLink(2, vertexShader, fragmentShader);
          pTS->mvpId = glGetUniformLocation(pTS->program, "mvp");
          if (pTS->mvpId < 0) {
            tsErrCallback(pTS->mvpId, "Unknown uniform mvp");
          };
          pTS->mapdataId = glGetUniformLocation(pTS->program, "mapdata");
          if (pTS->mapdataId < 0) {
            tsErrCallback(pTS->mapdataId, "Unknown uniform mapdata");
          };
          pTS->tileId = glGetUniformLocation(pTS->program, "tiles");
          if (pTS->tileId < 0) {
            tsErrCallback(pTS->tileId, "Unknown uniform tiles");
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

// unloads a tile shader
void tsUnload(tileshader* pTS) {
  if (pTS->program != NO_SHADER) {
    glDeleteProgram(pTS->program);
    pTS->program = NO_SHADER;
  };
};

// renders our tiles using our tile shader
void tsRender(tileshader* pTS, const mat4* pProjection, const mat4* pView) {
  mat4 mvp;
  vec3 tmpvector;
  
  if (pTS->program != NO_SHADER) {
    glUseProgram(pTS->program);

    // set our model-view-projection matrix first
    mat4Copy(&mvp, pProjection);
    mat4Multiply(&mvp, pView);
    
    // and lastly scale our x and y as they are unified
    mat4Scale(&mvp, vec3Set(&tmpvector, 100.0, 100.0, 1.0));

    if (pTS->mvpId >= 0) {
      glUniformMatrix4fv(pTS->mvpId, 1, false, (const GLfloat *) mvp.m);      
    };
    
    // now tell it which textures to use
    if (pTS->mapdataId >= 0) {
  		glActiveTexture(GL_TEXTURE0);
  		glBindTexture(GL_TEXTURE_2D, pTS->mapdataTexture);
  		glUniform1i(pTS->mapdataId, 0);      
    };

    if (pTS->tileId >= 0) {
  		glActiveTexture(GL_TEXTURE1);
  		glBindTexture(GL_TEXTURE_2D, pTS->tileTexture);
  		glUniform1i(pTS->tileId, 1);      
    };
    
    // and draw our triangles
    glDrawArrays(GL_TRIANGLES, 0, 40 * 40 * 3 * 2);
  };
};

#endif

#endif
