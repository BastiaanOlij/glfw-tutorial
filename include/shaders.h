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
 * functions. It also requires math3d.h to be included 
 * before hand
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

// and handy defines
#define NO_SHADER 0xFFFFFFFF

enum shaderErrors {
  SHADER_ERR_UNKNOWN = -1,
  SHADER_ERR_NOCOMPILE = -2,
  SHADER_ERR_NOLINK = -3
};

// typedef to obtain standard information, note that not all ids need to be present
typedef struct shaderStdInfo {
  GLuint  program;
  GLint   projectionMatrixId;       // our projection matrix
  GLint   viewMatrixId;             // our view matrix
  GLint   modelMatrixId;            // our model matrix
  GLint   modelViewMatrixId;        // our model view matrix
  GLint   modelViewInverseId;       // inverse of our model view matrix
  GLint   normalMatrixId;           // matrix to apply to our normals
  GLint   mvpId;                    // our model view projection matrix
} shaderStdInfo;

#ifdef __cplusplus
extern "C" {
#endif
  
typedef void(* ShaderError)(int, const char*, ...);

void shaderSetErrorCallback(ShaderError pCallback);
GLuint shaderCompile(GLenum pShaderType, const GLchar* pShaderText);
GLuint shaderLink(GLuint pNumShaders, ...);
shaderStdInfo shaderGetStdInfo(GLuint pProgram);
void shaderSelectProgram(shaderStdInfo pInfo, mat4 * pProjection, mat4 * pView, mat4 * pModel);
  
#ifdef __cplusplus
};
#endif	

#ifdef SHADER_IMPLEMENTATION

ShaderError shaderErrCallback = NULL;

// sets our error callback method
void shaderSetErrorCallback(ShaderError pCallback) {
  shaderErrCallback = pCallback;
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
		if ((len > 1) && (shaderErrCallback != NULL)) {
			GLchar* compiler_log;
      
      // allocate enough space for our prefix and error
      compiler_log = (GLchar*) malloc(len+50);
      
      // write out our prefix first
      sprintf(compiler_log, "Error compiling %s shader: ", type);
      
      // append our error
			glGetShaderInfoLog(shader, len, 0, &compiler_log[strlen(compiler_log)]);
	
      // and inform our calling logic
      shaderErrCallback(SHADER_ERR_NOCOMPILE, compiler_log);
            
      free(compiler_log);
    } else if (shaderErrCallback != NULL) {
      char error[250];
      sprintf(error,"Unknown error compiling %s shader", type);
      shaderErrCallback(SHADER_ERR_UNKNOWN, error);
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
  		if ((len > 1) && (shaderErrCallback != NULL)) {
  			GLchar* compiler_log;
        
        // allocate enough space for our prefix and error
        compiler_log = (GLchar*) malloc(len+50);
        
        // write out our prefix first
        strcpy(compiler_log, "Error linking shader program: ");
        
        // append our error
  			glGetProgramInfoLog(program, len, 0, &compiler_log[strlen(compiler_log)]);
			
        // and inform our calling logic
        shaderErrCallback(SHADER_ERR_NOLINK, compiler_log);
			
  			free(compiler_log);
      } else if (shaderErrCallback != NULL) {
        char error[250];
        strcpy(error,"Unknown error linking shader program");
        shaderErrCallback(SHADER_ERR_UNKNOWN, error);
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

  info.projectionMatrixId = glGetUniformLocation(info.program, "projection");
  if (info.projectionMatrixId < 0) {
    shaderErrCallback(info.projectionMatrixId, "Unknown uniform projection");  // just log it, may not be a problem
  };

  info.viewMatrixId = glGetUniformLocation(info.program, "view");
  if (info.viewMatrixId < 0) {
    shaderErrCallback(info.viewMatrixId, "Unknown uniform view");  // just log it, may not be a problem
  };

  info.modelMatrixId = glGetUniformLocation(info.program, "model");
  if (info.modelMatrixId < 0) {
    shaderErrCallback(info.modelMatrixId, "Unknown uniform model");  // just log it, may not be a problem
  };

  info.modelViewMatrixId = glGetUniformLocation(info.program, "modelView");
  if (info.modelViewMatrixId < 0) {
    shaderErrCallback(info.modelViewMatrixId, "Unknown uniform modelView");  // just log it, may not be a problem
  };

  info.modelViewInverseId = glGetUniformLocation(info.program, "modelViewInverse");
  if (info.modelViewInverseId < 0) {
    shaderErrCallback(info.modelViewInverseId, "Unknown uniform modelViewInverse");  // just log it, may not be a problem
  };

  info.normalMatrixId = glGetUniformLocation(info.program, "normalMatrix");
  if (info.normalMatrixId < 0) {
    shaderErrCallback(info.normalMatrixId, "Unknown uniform normalMatrix");  // just log it, may not be a problem
  };

  info.mvpId = glGetUniformLocation(info.program, "mvp");
  if (info.mvpId < 0) {
    shaderErrCallback(info.mvpId, "Unknown uniform mvp");  // just log it, may not be a problem
  };
  
  return info;
};

void shaderSelectProgram(shaderStdInfo pInfo, mat4 * pProjection, mat4 * pView, mat4 * pModel) {
  mat4  modelView, modelViewInverse, mvp;
  mat3  normalMatrix;
  glUseProgram(pInfo.program);
  
  if (pInfo.projectionMatrixId >= 0) {
    glUniformMatrix4fv(pInfo.projectionMatrixId, 1, false, (const GLfloat *) pProjection->m);
  };

  if (pInfo.viewMatrixId >= 0) {
    glUniformMatrix4fv(pInfo.viewMatrixId, 1, false, (const GLfloat *) pView->m);
  };

  if (pInfo.modelMatrixId >= 0) {
    glUniformMatrix4fv(pInfo.modelMatrixId, 1, false, (const GLfloat *) pModel->m);
  };
  
  mat4Copy(&modelView, pView);
  mat4Multiply(&modelView, pModel);

  if (pInfo.modelViewMatrixId >= 0) {
    glUniformMatrix4fv(pInfo.modelViewMatrixId, 1, false, (const GLfloat *) modelView.m);
  };

  // calculate the inverse of our model-view
  mat4Inverse(&modelViewInverse, &modelView);
  if (pInfo.modelViewInverseId >= 0) {
    glUniformMatrix4fv(pInfo.modelViewInverseId, 1, false, (const GLfloat *) modelViewInverse.m);
  };
  
  // now transpose our model-view inverse matrix
//  mat4Transpose(&modelViewInverse);
  // now get just the rotation part
//  mat3FromMat4(&normalMatrix, &modelViewInverse);

  // well the above does not seem to work very well...
  mat3FromMat4(&normalMatrix, &modelView);
  
  if (pInfo.normalMatrixId >= 0) {
    glUniformMatrix3fv(pInfo.normalMatrixId, 1, false, (const GLfloat *) normalMatrix.m);
  };

  mat4Copy(&mvp, pProjection);
  mat4Multiply(&mvp, &modelView);
  
  if (pInfo.mvpId >= 0) {
    glUniformMatrix4fv(pInfo.mvpId, 1, false, (const GLfloat *) mvp.m);
  };
};

#endif /* SHADER_IMPLEMENTATION */

#endif /* !shadersh */