/********************************************************
 * shaders.h - shader library by Bastiaan Olij 2015
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
#include "errorlog.h"
#include "math3d.h"
#include "material.h"

// and handy defines
#define NO_SHADER 0xFFFFFFFF

enum shaderErrors {
  SHADER_ERR_UNKNOWN = -1,
  SHADER_ERR_NOCOMPILE = -2,
  SHADER_ERR_NOLINK = -3
};

// typedef to obtain standard information, note that not all ids need to be present
typedef struct shaderStdInfo {
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
  GLint         lightPosId;         // position of our light

  // material
  GLint         alphaId;            // alpha
  GLint         matColorId;         // material diffuse color
  GLint         matSpecColorId;     // material specular color
  GLint         shininessId;        // shininess
  GLint         textureMapId;       // texture map
  GLint         reflectMapId;       // reflect map
} shaderStdInfo;

// and a matching structure to hold our matrices
typedef struct shaderMatrices {
  mat4  projection;                 // projection matrix
  mat4  view;                       // view matrix
  mat4  model;                      // model matrix
} shaderMatrices;

// and a structure to hold information about a light
typedef struct lightSource {
  vec3  position;                   // position of our light
  vec3  adjPosition;                // position of our light with view matrix applied
} lightSource;

#ifdef __cplusplus
extern "C" {
#endif
  
GLuint shaderCompile(GLenum pShaderType, const GLchar* pShaderText);
GLuint shaderLink(GLuint pNumShaders, ...);
shaderStdInfo shaderGetStdInfo(GLuint pProgram);
void shaderSelectProgram(shaderStdInfo * pInfo, shaderMatrices * pMatrices, lightSource * pLight, material * pMat);
  
#ifdef __cplusplus
};
#endif	

#ifdef SHADER_IMPLEMENTATION

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
  	GLint	linked = 0;

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

shaderStdInfo shaderGetStdInfo(GLuint pProgram) {
  shaderStdInfo info;
  
  info.program = pProgram;
  
  // camera info
  info.eyePosId = glGetUniformLocation(info.program, "eyePos");
  if (info.eyePosId < 0) {
    errorlog(info.eyePosId, "Unknown uniform eyePos");  // just log it, may not be a problem
  };
  
  // matrix info

  info.projectionMatrixId = glGetUniformLocation(info.program, "projection");
  if (info.projectionMatrixId < 0) {
    errorlog(info.projectionMatrixId, "Unknown uniform projection");  // just log it, may not be a problem
  };

  info.viewMatrixId = glGetUniformLocation(info.program, "view");
  if (info.viewMatrixId < 0) {
    errorlog(info.viewMatrixId, "Unknown uniform view");  // just log it, may not be a problem
  };

  info.modelMatrixId = glGetUniformLocation(info.program, "model");
  if (info.modelMatrixId < 0) {
    errorlog(info.modelMatrixId, "Unknown uniform model");  // just log it, may not be a problem
  };

  info.modelViewMatrixId = glGetUniformLocation(info.program, "modelView");
  if (info.modelViewMatrixId < 0) {
    errorlog(info.modelViewMatrixId, "Unknown uniform modelView");  // just log it, may not be a problem
  };

  info.modelViewInverseId = glGetUniformLocation(info.program, "modelViewInverse");
  if (info.modelViewInverseId < 0) {
    errorlog(info.modelViewInverseId, "Unknown uniform modelViewInverse");  // just log it, may not be a problem
  };

  info.normalMatrixId = glGetUniformLocation(info.program, "normalMatrix");
  if (info.normalMatrixId < 0) {
    errorlog(info.normalMatrixId, "Unknown uniform normalMatrix");  // just log it, may not be a problem
  };

  info.normalViewId = glGetUniformLocation(info.program, "normalView");
  if (info.normalViewId < 0) {
    errorlog(info.normalViewId, "Unknown uniform normalView");  // just log it, may not be a problem
  };

  info.mvpId = glGetUniformLocation(info.program, "mvp");
  if (info.mvpId < 0) {
    errorlog(info.mvpId, "Unknown uniform mvp");  // just log it, may not be a problem
  };
  
  // light
  
  info.lightPosId = glGetUniformLocation(info.program, "lightPos");
  if (info.lightPosId < 0) {
    errorlog(info.lightPosId, "Unknown uniform lightPos");
  };

  // material
  info.alphaId = glGetUniformLocation(info.program, "alpha");
  if (info.alphaId < 0) {
    errorlog(info.alphaId, "Unknown uniform alpha");    
  };

  info.matColorId = glGetUniformLocation(info.program, "matColor");
  if (info.matColorId < 0) {
    errorlog(info.matColorId, "Unknown uniform matColor");    
  };

  info.matSpecColorId = glGetUniformLocation(info.program, "matSpecColor");
  if (info.matSpecColorId < 0) {
    errorlog(info.matSpecColorId, "Unknown uniform matSpecColor");    
  };
  
  info.shininessId = glGetUniformLocation(info.program, "shininess");
  if (info.shininessId < 0) {
    errorlog(info.textureMapId, "Unknown uniform shininess");    
  };
  
  info.textureMapId = glGetUniformLocation(info.program, "textureMap");
  if (info.textureMapId < 0) {
    errorlog(info.textureMapId, "Unknown uniform textureMap");
  };
  
  info.reflectMapId = glGetUniformLocation(info.program, "reflectMap");
  if (info.reflectMapId < 0) {
    errorlog(info.reflectMapId, "Unknown uniform reflectMap");
  };
  
  return info;
};

void shaderSelectProgram(shaderStdInfo * pInfo, shaderMatrices * pMatrices, lightSource * pLight, material * pMat) {
  mat4  modelView, modelViewInverse, mvp;
  int   texture = 0;
  
  glUseProgram(pInfo->program);
  
  // setup camera info
  
  if (pInfo->eyePosId >= 0) {
    glUniform3f(pInfo->eyePosId, -pMatrices->view.m[3][0], -pMatrices->view.m[3][1], -pMatrices->view.m[3][2]);       
  };
  
  // setup our matrices
  
  if (pInfo->projectionMatrixId >= 0) {
    glUniformMatrix4fv(pInfo->projectionMatrixId, 1, false, (const GLfloat *) pMatrices->projection.m);
  };

  if (pInfo->viewMatrixId >= 0) {
    glUniformMatrix4fv(pInfo->viewMatrixId, 1, false, (const GLfloat *) pMatrices->view.m);
  };

  if (pInfo->modelMatrixId >= 0) {
    glUniformMatrix4fv(pInfo->modelMatrixId, 1, false, (const GLfloat *) pMatrices->model.m);
  };
  
  mat4Copy(&modelView, &pMatrices->view);
  mat4Multiply(&modelView, &pMatrices->model);

  if (pInfo->modelViewMatrixId >= 0) {
    glUniformMatrix4fv(pInfo->modelViewMatrixId, 1, false, (const GLfloat *) modelView.m);
  };

  // calculate the inverse of our model-view
  mat4Inverse(&modelViewInverse, &modelView);
  if (pInfo->modelViewInverseId >= 0) {
    glUniformMatrix4fv(pInfo->modelViewInverseId, 1, false, (const GLfloat *) modelViewInverse.m);
  };
  
  // our normal matrix taken from our model matrix
  if (pInfo->normalMatrixId >= 0) {
    mat3  matrix;
    mat3FromMat4(&matrix, &pMatrices->model);
    glUniformMatrix3fv(pInfo->normalMatrixId, 1, false, (const GLfloat *) matrix.m);
  };

  // our normal matrix taken from our modelView matrix
  if (pInfo->normalViewId >= 0) {
    mat3  matrix;
    mat3FromMat4(&matrix, &modelView);
    glUniformMatrix3fv(pInfo->normalViewId, 1, false, (const GLfloat *) matrix.m);
  };

  mat4Copy(&mvp, &pMatrices->projection);
  mat4Multiply(&mvp, &modelView);
  
  if (pInfo->mvpId >= 0) {
    glUniformMatrix4fv(pInfo->mvpId, 1, false, (const GLfloat *) mvp.m);
  };
  
  // setup our light

  if (pInfo->lightPosId >= 0) {
    glUniform3f(pInfo->lightPosId, pLight->adjPosition.x, pLight->adjPosition.y, pLight->adjPosition.z);       
  };

  // setup our material
  if (pInfo->alphaId >= 0) {
    if (pMat == NULL) {
      glUniform1f(pInfo->alphaId, 1.0);
    } else {
      glUniform1f(pInfo->alphaId, pMat->alpha);      
    };
  };

  if (pInfo->matColorId >= 0) {
    if (pMat == NULL) {
      glUniform3f(pInfo->matColorId, 1.0, 1.0, 1.0);
    } else {
      glUniform3f(pInfo->matColorId, pMat->matColor.x, pMat->matColor.y, pMat->matColor.z);      
    };
  };

  if (pInfo->matSpecColorId >= 0) {
    if (pMat == NULL) {
      glUniform3f(pInfo->matSpecColorId, 1.0, 1.0, 1.0);
    } else {
      glUniform3f(pInfo->matSpecColorId, pMat->matSpecColor.x, pMat->matSpecColor.y, pMat->matSpecColor.z);      
    };
  };
  
  if (pInfo->shininessId >= 0) {
    if (pMat == NULL) {
      glUniform1f(pInfo->shininessId, 50.0);
    } else {
      glUniform1f(pInfo->shininessId, pMat->shininess);      
    };
  };

  if (pInfo->textureMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else if (pMat->textureId == MAT_NO_TEXTURE) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->textureId);      
    }
    glUniform1i(pInfo->textureMapId, texture); 
    texture++;   
  };

  if (pInfo->reflectMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else if (pMat->reflectId == MAT_NO_TEXTURE) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->reflectId);      
    }
    glUniform1i(pInfo->reflectMapId, texture); 
    texture++;   
  };
};

#endif /* SHADER_IMPLEMENTATION */

#endif /* !shadersh */