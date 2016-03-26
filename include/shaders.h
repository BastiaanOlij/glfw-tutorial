  /********************************************************
 * shaders.h - shader library by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define SHADER_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions.
 * 
 * This library does not contain any logic to load
 * shaders from disk.
 *
 * Revision history:
 * 0.1  09-03-2015  First version with basic functions
 * 0.2  06-10-2016  Moved shaderSelectProgram into materials
 *
 ********************************************************/

#ifndef shadersh
#define shadersh

// standard libraries we need...
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// our libraries we need
#include "system.h"
#include "math3d.h"

// and handy defines
#define NO_SHADER 0xFFFFFFFF

enum shaderErrors {
  SHADER_ERR_UNKNOWN = -1,
  SHADER_ERR_NOCOMPILE = -2,
  SHADER_ERR_NOLINK = -3
};

// structure for encapsulating a shader, note that not all ids need to be present (would be logical to call this struct shader but it's already used in some of the support libraries...)
typedef struct shaderInfo {
  unsigned int  retainCount;        // retain count for this object
  char    name[50];                 // name of the shader
  GLuint  program;                  // shader program to use
  
  // camera info
  GLint   eyePosId;                 // our eye position
  
  // matrices
  GLint   projectionMatrixId;       // our projection matrix
  GLint   viewMatrixId;             // our view matrix
  GLint   modelMatrixId;            // our model matrix
  GLint   modelViewMatrixId;        // our model view matrix
  GLint   modelViewInverseId;       // inverse of our model view matrix
  GLint   normalMatrixId;           // matrix to apply to our normals
  GLint   normalViewId;             // matrix to apply to our normals also adjusted to view
  GLint   mvpId;                    // our model view projection matrix

  // light info  
  GLint   ambientId;                // ambient color of our light
  GLint   lightPosId;               // position of our light

  // material
  GLint   alphaId;                  // alpha
  GLint   matColorId;               // material diffuse color
  GLint   matSpecColorId;           // material specular color
  GLint   shininessId;              // shininess
  GLint   textureMapId;             // texture map
  GLint   reflectMapId;             // reflect map
  GLint   bumpMapId;                // bump map (used as normal map or height map)
} shaderInfo;

// and a matching structure to hold our matrices, do not set any of these directly but always use the methods given below as we're lazy updating alot of these!!
typedef struct shaderMatrices {
  mat4    projection;               // projection matrix
  mat4    view;                     // view matrix
  mat4    model;                    // model matrix

  // calculated
  bool    updViewProj;              // need to update our view projection matrix?
  mat4    viewProj;                 // view projection matrix

  bool    updInvView;               // need to update the inverse of our view?
  mat4    invView;                  // inverse of our view

  bool    updModelView;             // need to update our model view matrix
  mat4    modelView;                // our model view matrix

  bool    updInvModelView;          // need to update our inverse model view matrix
  mat4    invModelView;             // our inverse model view matrix

  bool    updMvp;                   // need to update our model view projection matrix
  mat4    mvp;                      // our model view matrix projection

  bool    updNormal;                // need to update our normal matrix
  mat3    normal;                   // our normal matrix

  bool    updNormView;              // need to update our normal view matrix
  mat3    normalView;               // our normal view matrix
} shaderMatrices;

// and a structure to hold information about a light
typedef struct lightSource {
  float   ambient;                  // ambient factor for our light
  vec3    position;                 // position of our light
  vec3    adjPosition;              // position of our light with view matrix applied
} lightSource;

#ifdef __cplusplus
extern "C" {
#endif
  
// support functions
void shaderSetPath(char * pPath);
GLuint shaderCompile(GLenum pShaderType, const GLchar* pShaderText);
GLuint shaderLoad(GLenum pShaderType, const char *pName);
GLuint shaderLink(GLuint pNumShaders, ...);

// shader object
shaderInfo * newShader(const char *pName, const char * pVertexShader, const char * pTessControlShader, const char * pTessEvalShader, const char * pGeoShader, const char * pFragmentShader);
void shaderRetain(shaderInfo * pShader);
void shaderRelease(shaderInfo * pShader);
void shaderSetProgram(shaderInfo * pShader, GLuint pProgram);

// shader matrix structure
void shdMatSetProjection(shaderMatrices * pShdMat, const mat4 * pProjection);
void shdMatSetView(shaderMatrices * pShdMat, const mat4 * pView);
void shdMatSetModel(shaderMatrices * pShdMat, const mat4 * pModel);
mat4 * shdMatGetViewProjection(shaderMatrices * pShdMat);
mat4 * shdMatGetInvView(shaderMatrices * pShdMat);
vec3 * shdMatGetEyePos(shaderMatrices * pShdMat, vec3 * pEyePos);
mat4 * shdMatGetModelView(shaderMatrices * pShdMat);
mat4 * shdMatGetInvModelView(shaderMatrices * pShdMat);
mat4 * shdMatGetMvp(shaderMatrices * pShdMat);
mat3 * shdMatGetNormal(shaderMatrices * pShdMat);
mat3 * shdMatGetNormalView(shaderMatrices * pShdMat);

#ifdef __cplusplus
};
#endif  

#ifdef SHADER_IMPLEMENTATION

// some variables we maintain
char shaderPath[1024] = "";

// sets the locationf from which we load our texture maps
void shaderSetPath(char * pPath) {
  strcpy(shaderPath, pPath);
};

// Compiles the text in pShaderText and returns a shader object
// pShaderType defines what type of shader we are compiling
// i.e. GL_VERTEX_SHADER
// On failure returns NO_SHADER
// On success returns a shader ID that can be used to link our program. 
// Note that you must discard the shader ID with glDeleteShader
// You can do this after a program has been successfully compiled
GLuint shaderCompile(GLenum pShaderType, const GLchar * pShaderText) {
  GLint compiled = 0;
  GLuint shader;
  const GLchar *stringptrs[1];
  
  // create our shader
  shader = glCreateShader(pShaderType);
  if (shader == 0) {
    errorlog(SHADER_ERR_NOCOMPILE, "Unable to create shader object");
    return NO_SHADER;
  } else if (shader == GL_INVALID_ENUM) {
    errorlog(SHADER_ERR_NOCOMPILE, "Incorrect or unsupported shader type specified");
    return NO_SHADER;    
  };
  
  // compile our shader
  stringptrs[0] = pShaderText;
  glShaderSource(shader, 1, stringptrs, NULL);
  glCompileShader(shader);
  
  // check our status
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) { 
    GLint len = 0;
    char type[50];

    switch (pShaderType) {
      case GL_VERTEX_SHADER: {
        strcpy(type, "vertex");
      } break;
      case GL_TESS_CONTROL_SHADER: {
        strcpy(type, "tessellation control");
      } break;
      case GL_TESS_EVALUATION_SHADER: {
        strcpy(type, "tessellation evaluation");
      } break;
      case GL_GEOMETRY_SHADER: {
        strcpy(type, "geometry");
      } break;
      case GL_FRAGMENT_SHADER: {
        strcpy(type, "fragment");
      } break;
      default: {
        strcpy(type, "unknown");
      } break;
    };

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &len); 
    if (len > 1) {
      GLchar* compiler_log;
      
      // allocate enough space for our prefix and error
      compiler_log = (GLchar*) malloc(len+50);
      
      // write out our prefix first
      sprintf(compiler_log, "Error compiling %s shader: ", type);
      
      // append our error
      glGetShaderInfoLog(shader, len, 0, &compiler_log[strlen(compiler_log)]);
  
      // and inform our calling logic
      errorlog(SHADER_ERR_NOCOMPILE, compiler_log);
            
      free(compiler_log);
    } else {
      char error[250];
      sprintf(error,"Unknown error compiling %s shader", type);
      errorlog(SHADER_ERR_UNKNOWN, error);
    };
        
    glDeleteShader(shader);
    shader = NO_SHADER;
  };
  
  return shader;
};

GLuint shaderLoad(GLenum pShaderType, const char *pName) {
  GLuint      shader = NO_SHADER;
  char *      shaderText = NULL;

  shaderText = loadFile(shaderPath, pName);
  if (shaderText != NULL) {
    shader = shaderCompile(pShaderType, shaderText);

    free(shaderText);
  };

  return shader;
};

// Links any number of programs into a shader program
// To compile and link a shader:
// ----
// GLuint vertexShader, fragmentShader, shaderProgram;
// vertexShader = shaderCompile(GL_VERTEX_SHADER, vsText);
// fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, vsText);
// shaderProgram = shaderLink(2, vertexShader, fragmentShader);
// glDeleteShader(vertexShader);
// glDeleteShader(fragmentShader);
// ----
// Returns NO_SHADER on failure
// Returns program ID on success
// You must call glDeleteProgram to cleanup the program after you are done.
GLuint shaderLink(GLuint pNumShaders, ...) {
  GLuint program;
  va_list shaders;
  int s;
  
  // create our shader program...
  program = glCreateProgram();
  
  // now add our compiled code...
  va_start(shaders, pNumShaders);
  
  for (s = 0; s < pNumShaders && program != NO_SHADER; s++) {
    GLuint shader = va_arg(shaders, GLuint);
    
    if (shader == NO_SHADER) {
      // assume we've set our error when the shader failed to compile...
      glDeleteProgram(program);
      program = NO_SHADER;
    } else {
      glAttachShader(program, shader);
    };
  };
  
  va_end(shaders);
  
  // and try and link our program
  if (program != NO_SHADER) {
    GLint linked = 0;

    glLinkProgram(program);

    // and check whether it all went OK..
    glGetProgramiv(program, GL_LINK_STATUS, &linked);   
    if (!linked) {
      GLint len = 0;

      glGetProgramiv(program, GL_INFO_LOG_LENGTH , &len); 
      if (len > 1) {
        GLchar* compiler_log;
        
        // allocate enough space for our prefix and error
        compiler_log = (GLchar*) malloc(len+50);
        
        // write out our prefix first
        strcpy(compiler_log, "Error linking shader program: ");
        
        // append our error
        glGetProgramInfoLog(program, len, 0, &compiler_log[strlen(compiler_log)]);
      
        // and inform our calling logic
        errorlog(SHADER_ERR_NOLINK, compiler_log);
      
        free(compiler_log);
      } else {
        char error[250];
        strcpy(error,"Unknown error linking shader program");
        errorlog(SHADER_ERR_UNKNOWN, error);
      };
    
      glDeleteProgram(program);
      program = NO_SHADER;
    };    
  };
  
  return program;
};

////////////////////////////////////////////////////////////////////////////////////
// shader

shaderInfo * newShader(const char *pName, const char * pVertexShader, const char * pTessControlShader, const char * pTessEvalShader, const char * pGeoShader, const char * pFragmentShader) {
  shaderInfo * newshader = (shaderInfo *)malloc(sizeof(shaderInfo));
  if (newshader != NULL) {
    char    filename[1024];
    int     count = 0;
    GLuint  shaders[5];

    memset(newshader, 255, sizeof(shaderInfo));
    newshader->retainCount = 1;
    newshader->program = NO_SHADER;
    strcpy(newshader->name, pName);

    // attempt to load our shader by name
    if (pVertexShader != NULL) {
      shaders[count] = shaderLoad(GL_VERTEX_SHADER, pVertexShader);
      if (shaders[count] != NO_SHADER) count++;      
    };
    if (pTessControlShader != NULL) {
      shaders[count] = shaderLoad(GL_TESS_CONTROL_SHADER, pTessControlShader);
      if (shaders[count] != NO_SHADER) count++;
    };
    if (pTessEvalShader != NULL) {
      shaders[count] = shaderLoad(GL_TESS_EVALUATION_SHADER, pTessEvalShader);
      if (shaders[count] != NO_SHADER) count++;
    };
    if (pGeoShader != NULL) {
      shaders[count] = shaderLoad(GL_GEOMETRY_SHADER, pGeoShader);
      if (shaders[count] != NO_SHADER) count++;
    };
    if (pFragmentShader != NULL) {
      shaders[count] = shaderLoad(GL_FRAGMENT_SHADER, pFragmentShader);
      if (shaders[count] != NO_SHADER) count++;
    };

    // attempt to compile our shader
    if (count>0) {
      shaderSetProgram(newshader, shaderLink(count, shaders[0], shaders[1], shaders[2], shaders[3], shaders[4]));

      // and cleanup..
      while (count>0) {
        count--;
        glDeleteShader(shaders[count]);
      };
    };
  };
  return newshader;
};

void shaderRetain(shaderInfo * pShader) {
  if (pShader != NULL) {
    pShader->retainCount++;
  };
};

void shaderRelease(shaderInfo * pShader) {
  if (pShader == NULL) {
    return;
  } else if (pShader->retainCount > 1) {
    pShader->retainCount--;

//    errorlog(0, "Decreased retain count %p to %i", pShader, pShader->retainCount);
    
    return;
  };

  if (pShader->program != NO_SHADER) {
    glDeleteProgram(pShader->program);
  };

  free(pShader);
};

void shaderSetProgram(shaderInfo * pShader, GLuint pProgram) {
  pShader->program = pProgram;
  
  // camera info
  pShader->eyePosId = glGetUniformLocation(pShader->program, "eyePos");
  if (pShader->eyePosId < 0) {
    errorlog(pShader->eyePosId, "Unknown uniform %s:eyePos", pShader->name);  // just log it, may not be a problem
  };
  
  // matrix info

  pShader->projectionMatrixId = glGetUniformLocation(pShader->program, "projection");
  if (pShader->projectionMatrixId < 0) {
    errorlog(pShader->projectionMatrixId, "Unknown uniform %s:projection", pShader->name);  // just log it, may not be a problem
  };

  pShader->viewMatrixId = glGetUniformLocation(pShader->program, "view");
  if (pShader->viewMatrixId < 0) {
    errorlog(pShader->viewMatrixId, "Unknown uniform %s:view", pShader->name);  // just log it, may not be a problem
  };

  pShader->modelMatrixId = glGetUniformLocation(pShader->program, "model");
  if (pShader->modelMatrixId < 0) {
    errorlog(pShader->modelMatrixId, "Unknown uniform %s:model", pShader->name);  // just log it, may not be a problem
  };

  pShader->modelViewMatrixId = glGetUniformLocation(pShader->program, "modelView");
  if (pShader->modelViewMatrixId < 0) {
    errorlog(pShader->modelViewMatrixId, "Unknown uniform %s:modelView", pShader->name);  // just log it, may not be a problem
  };

  pShader->modelViewInverseId = glGetUniformLocation(pShader->program, "modelViewInverse");
  if (pShader->modelViewInverseId < 0) {
    errorlog(pShader->modelViewInverseId, "Unknown uniform %s:modelViewInverse", pShader->name);  // just log it, may not be a problem
  };

  pShader->normalMatrixId = glGetUniformLocation(pShader->program, "normalMatrix");
  if (pShader->normalMatrixId < 0) {
    errorlog(pShader->normalMatrixId, "Unknown uniform %s:normalMatrix", pShader->name);  // just log it, may not be a problem
  };

  pShader->normalViewId = glGetUniformLocation(pShader->program, "normalView");
  if (pShader->normalViewId < 0) {
    errorlog(pShader->normalViewId, "Unknown uniform %s:normalView", pShader->name);  // just log it, may not be a problem
  };

  pShader->mvpId = glGetUniformLocation(pShader->program, "mvp");
  if (pShader->mvpId < 0) {
    errorlog(pShader->mvpId, "Unknown uniform %s:mvp", pShader->name);  // just log it, may not be a problem
  };
  
  // light
  
  pShader->lightPosId = glGetUniformLocation(pShader->program, "lightPos");
  if (pShader->lightPosId < 0) {
    errorlog(pShader->lightPosId, "Unknown uniform %s:lightPos", pShader->name);
  };
  pShader->ambientId = glGetUniformLocation(pShader->program, "ambient");
  if (pShader->ambientId < 0) {
    errorlog(pShader->ambientId, "Unknown uniform %s:ambient", pShader->name);
  };

  // material
  pShader->alphaId = glGetUniformLocation(pShader->program, "alpha");
  if (pShader->alphaId < 0) {
    errorlog(pShader->alphaId, "Unknown uniform %s:alpha", pShader->name);    
  };

  pShader->matColorId = glGetUniformLocation(pShader->program, "matColor");
  if (pShader->matColorId < 0) {
    errorlog(pShader->matColorId, "Unknown uniform %s:matColor", pShader->name);    
  };

  pShader->matSpecColorId = glGetUniformLocation(pShader->program, "matSpecColor");
  if (pShader->matSpecColorId < 0) {
    errorlog(pShader->matSpecColorId, "Unknown uniform %s:matSpecColor", pShader->name);    
  };
  
  pShader->shininessId = glGetUniformLocation(pShader->program, "shininess");
  if (pShader->shininessId < 0) {
    errorlog(pShader->shininessId, "Unknown uniform %s:shininess", pShader->name);    
  };
  
  pShader->textureMapId = glGetUniformLocation(pShader->program, "textureMap");
  if (pShader->textureMapId < 0) {
    errorlog(pShader->textureMapId, "Unknown uniform %s:textureMap", pShader->name);
  };
  
  pShader->reflectMapId = glGetUniformLocation(pShader->program, "reflectMap");
  if (pShader->reflectMapId < 0) {
    errorlog(pShader->reflectMapId, "Unknown uniform %s:reflectMap", pShader->name);
  };

  pShader->bumpMapId = glGetUniformLocation(pShader->program, "bumpMap");
  if (pShader->bumpMapId < 0) {
    errorlog(pShader->bumpMapId, "Unknown uniform %s:bumpMap", pShader->name);
  };
};

////////////////////////////////////////////////////////////////////////////////////
// shader matrices

void shdMatSetProjection(shaderMatrices * pShdMat, const mat4 * pProjection) {
  // need to improve this to skip if our matrix isn't changing
  mat4Copy(&pShdMat->projection, pProjection);
  pShdMat->updMvp = true;
  pShdMat->updViewProj = true;
};

void shdMatSetView(shaderMatrices * pShdMat, const mat4 * pView) {
  // need to improve this to skip if our matrix isn't changing
  mat4Copy(&pShdMat->view, pView);
  pShdMat->updViewProj = true;
  pShdMat->updInvView = true;
  pShdMat->updModelView = true;
  pShdMat->updInvModelView = true;
  pShdMat->updMvp = true;
  pShdMat->updNormView = true;
};

void shdMatSetModel(shaderMatrices * pShdMat, const mat4 * pModel) {
  // need to improve this to skip if our matrix isn't changing
  mat4Copy(&pShdMat->model, pModel);
  pShdMat->updInvView = true;
  pShdMat->updModelView = true;
  pShdMat->updInvModelView = true;
  pShdMat->updMvp = true;
  pShdMat->updNormal = true;
  pShdMat->updNormView = true;
};

mat4 * shdMatGetViewProjection(shaderMatrices * pShdMat) {
  if (pShdMat->updViewProj) {
    mat4Copy(&pShdMat->viewProj, &pShdMat->projection);
    mat4Multiply(&pShdMat->viewProj, &pShdMat->view);

    pShdMat->updViewProj = false;
  };
  
  return &pShdMat->viewProj;
};

mat4 * shdMatGetInvView(shaderMatrices * pShdMat) {
  if (pShdMat->updInvView) {
    mat4Inverse(&pShdMat->invView, &pShdMat->view);
    pShdMat->updInvView = false;
  };

  return &pShdMat->invView;
};

vec3 * shdMatGetEyePos(shaderMatrices * pShdMat, vec3 * pEyePos) {
  mat4 * tmpmatrix = shdMatGetInvView(pShdMat);
  vec3Set(pEyePos, tmpmatrix->m[3][0], tmpmatrix->m[3][1], tmpmatrix->m[3][2]);

  return pEyePos;
};

mat4 * shdMatGetModelView(shaderMatrices * pShdMat) {
  if (pShdMat->updModelView) {
    mat4Copy(&pShdMat->modelView, &pShdMat->view);
    mat4Multiply(&pShdMat->modelView, &pShdMat->model);

    pShdMat->updModelView = true;
  };
  return &pShdMat->modelView;
};

mat4 * shdMatGetInvModelView(shaderMatrices * pShdMat) {
  if (pShdMat->updInvModelView) {
    mat4Inverse(&pShdMat->invModelView, shdMatGetModelView(pShdMat));

    pShdMat->updInvModelView = false;
  };

  return &pShdMat->invModelView;
};

mat4 * shdMatGetMvp(shaderMatrices * pShdMat) {
  if (pShdMat->updMvp) {
    mat4Copy(&pShdMat->mvp, &pShdMat->projection);
    mat4Multiply(&pShdMat->mvp, shdMatGetModelView(pShdMat));

    pShdMat->updMvp = false;
  };

  return &pShdMat->mvp;
};

mat3 * shdMatGetNormal(shaderMatrices * pShdMat) {
  if (pShdMat->updNormal) {
    mat3FromMat4(&pShdMat->normal, shdMatGetModelView(pShdMat));
    pShdMat->updNormal = false;
  };

  return &pShdMat->normal;
};

mat3 * shdMatGetNormalView(shaderMatrices * pShdMat) {
  if (pShdMat->updNormView) {
    mat3FromMat4(&pShdMat->normalView, shdMatGetModelView(pShdMat));
    pShdMat->updNormView = false;
  };

  return &pShdMat->normalView;
};


#endif /* SHADER_IMPLEMENTATION */

#endif /* !shadersh */