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

// and handy defines
#define NO_SHADER 0xFFFFFFFF

enum shaderErrors {
  SHADER_ERR_UNKNOWN = -1,
  SHADER_ERR_NOCOMPILE = -2,
  SHADER_ERR_NOLINK = -3
};

#ifdef __cplusplus
extern "C" {
#endif
  
typedef void(* ShaderError)(int, const char*, ...);

void shaderSetErrorCallback(ShaderError pCallback);
GLuint shaderCompile(GLenum pShaderType, const GLchar* pShaderText);
GLuint shaderLink(GLuint pNumShaders, ...);
  
#ifdef __cplusplus
};
#endif	

#ifdef SHADER_IMPLEMENTATION

ShaderError shaderErrCallback = NULL;

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
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


#endif

#endif