// include GLEW
#include <GL/glew.h>

// include these defines to let GLFW know we need OpenGL 3 support 
#define GLFW_INCLUDE_GL_3
#include <GLFW/glfw3.h>

// include some standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
#include <syslog.h>
#endif

// include support libraries including their implementation
#define MATH3D_IMPLEMENTATION
#include "math3d.h"
#define SHADER_IMPLEMENTATION
#include "shaders.h"

// For now just some global state to store our shader program and buffers
GLuint shaderProgram = NO_SHADER;
GLuint VAO = 0;
GLuint VBOs[2] = { 0, 0 };

// function for logging errors
void error_callback(int error, const char* description) {
#ifdef __APPLE__
	// output to syslog
	syslog(LOG_ALERT, "%i: %s", error, description);  
#elif WIN32
  // output to stderr
  fprintf(stderr, "%i: %s", error, description);
#else
  // not sure what we're doing on other platforms yet
#endif
};

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
    error_callback(-100, error);
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
      error_callback(-101, error);    
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
  shaderSetErrorCallback(error_callback);
  
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

void load_objects() {
  // data for our triangle
  GLfloat vertices[9] = {
    -0.6f, -0.4f,  0.0f,
     0.6f, -0.4f,  0.0f,
     0.0f,  0.6f,  0.0f
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
};

int main(void) {
  GLFWwindow* window;
  
  // tell GLFW how to inform us of issues
  glfwSetErrorCallback(error_callback);

  // see if we can initialize GLFW
  if (!glfwInit()) {
    exit(EXIT_FAILURE);    
  };
  
  // make sure we're using OpenGL 3.2+
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // create our window
  window = glfwCreateWindow(640, 480, "GLFW Tutorial", NULL, NULL);
  if (window) {
    GLenum err;
	
    // make our context current
    glfwMakeContextCurrent(window);
 
    // init GLEW
    glewExperimental=true;
    err = glewInit();
    if (err != GLEW_OK) {
      error_callback(err, glewGetErrorString(err)); 
	  exit(EXIT_FAILURE); 
    };

    // tell GLFW how to inform us of keyboard input
    glfwSetKeyCallback(window, key_callback);
    
    // load, compile and link our shader(s)
    load_shaders();
    
    // load our objects
    load_objects();
            
    // and start our render loop
    while (!glfwWindowShouldClose(window)) {
      float ratio;
      int width, height;
      mat4 mvp;
      vec3 axis;
      
      glfwGetFramebufferSize(window, &width, &height);
      ratio = width / (float) height;
      
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);      
      
      // set our model view projection matrix

      // glMatrixMode(GL_PROJECTION);
      // glLoadIdentity();
      // glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4Identity(&mvp);
      mat4Ortho(&mvp, -ratio, ratio, -1.0f, 1.0f, 1.0f, -1.0f);

      // glMatrixMode(GL_MODELVIEW);
      // glLoadIdentity();
      // glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
      mat4Rotate(&mvp, (float) glfwGetTime() * 50.0f, vec3Set(&axis, 0.0f, 0.0f, 1.0f));
            
      // select our shader
      if (shaderProgram != NO_SHADER) {
        GLint mvpId;

        glUseProgram(shaderProgram);
        
        // should retrieve our mvpId once and reuse
        mvpId = glGetUniformLocation(shaderProgram, "mvp");
        if (mvpId >= 0) {
          glUniformMatrix4fv(mvpId, 1, false, (const GLfloat *) mvp.m);
        } else {
          error_callback(mvpId, "Unknown uniform mvp");
        };
      };
      
      // make our VAO current
      glBindVertexArray(VAO);

      // draw our triangles:)

      /*
      glBegin(GL_TRIANGLES);
      glColor3f(1.f, 0.f, 0.f);
      glVertex3f(-0.6f, -0.4f, 0.f);
      glColor3f(0.f, 1.f, 0.f);
      glVertex3f(0.6f, -0.4f, 0.f);
      glColor3f(0.f, 0.f, 1.f);
      glVertex3f(0.f, 0.6f, 0.f);
      glEnd();
      */
      
      glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
      
      // unset our VAO
      glBindVertexArray(0);
      
      // unset our shader
      glUseProgram(0);
      
      // swap our buffers around so the user sees our new frame
      glfwSwapBuffers(window);
      glfwPollEvents();
    };
  
    // close our window
    glfwDestroyWindow(window);  
  };
  
  // lets be nice and cleanup
  unload_objects();
  unload_shaders();
  
  // the end....
  glfwTerminate();
};
