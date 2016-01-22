/********************************************************
 * mesh3d.h - 3D mesh library by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define MESH_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. It also requires math3d.h to be included 
 * before hand
 *
 * Revision history:
 * 0.1  12-01-2016  First version with basic functions
 *
 ********************************************************/

#ifndef mesh3dh
#define mesh3dh

#define GL_UNDEF_OBJ      0xffffffff
#define BUFFER_EXPAND     100

// structure for our vertices
typedef struct vertex {
  vec3    V;          // position of our vertice (XYZ)
  vec3    N;          // normal of our vertice (XYZ)
  vec2    T;          // texture coordinates (XY)
} vertex;

// structure for encapsulating mesh data
typedef struct mesh3d {
  char          name[50];             /* name for this mesh */
  
  // mesh data
  GLuint        numVertices;          /* number of vertices in our object */
  GLuint        verticesSize;         /* size of our vertices array */
  vertex *      verticesData;         /* array with vertices, can be NULL once loaded into GPU memory */
  GLuint        numIndices;           /* number of indices in our object */
  GLuint        indicesSize;          /* size of our vertices array */
  GLuint *      indicesData;          /* array with indices, can be NULL once loaded into GPU memory */

  // GPU state
  GLuint        VAO;                  /* our vertex array object */
  GLuint        VBO[2];               /* our two vertex buffer objects */
} mesh3d;

#ifdef __cplusplus
extern "C" {
#endif

typedef void(* MeshError)(int, const char*, ...);

void meshSetErrorCallback(MeshError pCallback);

mesh3d * newMesh(GLuint pInitialVertices, GLuint pInitialIndices);
void meshFree(mesh3d * pMesh);
GLuint meshAddVertex(mesh3d * pMesh, const vertex * pVertex);
bool meshAddFace(mesh3d * pMesh, GLuint pA, GLuint pB, GLuint pC);
bool meshCopyToGL(mesh3d * pMesh, bool pFreeBuffers);
bool meshRender(mesh3d * pMesh);

bool meshMakeCube(mesh3d * pMesh, GLfloat pWidth, GLfloat pHeight, GLfloat pDepth);
bool meshMakeSphere(mesh3d * pMesh, GLfloat pRadius);

#ifdef __cplusplus
};
#endif  

#ifdef MESH_IMPLEMENTATION

MeshError meshErrCallback = NULL;

// sets our error callback method
void meshSetErrorCallback(MeshError pCallback) {
  meshErrCallback = pCallback;
};

// Initialize a new mesh that either has been allocated on the heap or allocated with
void meshInit(mesh3d * pMesh, GLuint pInitialVertices, GLuint pInitialIndices) {
  if (pMesh == NULL) {
    return;
  };
  
  strcpy(pMesh->name, "New");

  // init our vertices
  pMesh->numVertices = 0;
  pMesh->verticesData = pInitialVertices > 0 ? (vertex * ) malloc(sizeof(vertex) * pInitialVertices) : NULL;    
  pMesh->verticesSize = pMesh->verticesData != NULL ? pInitialVertices : 0;
  if ((pMesh->verticesData == NULL) && (pInitialVertices!=0)) {
    meshErrCallback(1, "Couldn''t allocate vertex array data");    
  };
  
  // init our indices
  pMesh->numIndices = 0;
  pMesh->indicesData = pInitialIndices > 0 ? (GLuint *) malloc (sizeof(GLuint) * pInitialIndices) : NULL;
  pMesh->indicesSize = pMesh->indicesData != NULL ? pInitialIndices : 0;
  if ((pMesh->indicesData == NULL) && (pInitialIndices!=0)) {
    meshErrCallback(2, "Couldn''t allocate index array data");    
  };
  
  pMesh->VAO = GL_UNDEF_OBJ;
  pMesh->VBO[0] = GL_UNDEF_OBJ;
  pMesh->VBO[1] = GL_UNDEF_OBJ;
};

mesh3d * newMesh(GLuint pInitialVertices, GLuint pInitialIndices) {
  mesh3d * mesh = (mesh3d *) malloc(sizeof(mesh3d));
  if (mesh == NULL) {
    meshErrCallback(1, "Couldn''t allocate memory for mesh");        
  } else {
    meshInit(mesh, pInitialVertices, pInitialIndices);
  };
  return mesh;
};

// frees up data and buffers associated with this mesh
void meshFree(mesh3d * pMesh) {
  if (pMesh == NULL) {
    return;
  };
  
  if (pMesh->verticesData != NULL) {
    free(pMesh->verticesData);
    pMesh->numVertices = 0;
    pMesh->verticesSize = 0;
    pMesh->verticesData = NULL;
  };

  if (pMesh->indicesData != NULL) {
    free(pMesh->indicesData);
    pMesh->numIndices = 0;
    pMesh->indicesSize = 0;
    pMesh->indicesData = NULL;
  };
  
  if (pMesh->VBO[0] != GL_UNDEF_OBJ) {
    // these are allocated in pairs so...
    glDeleteBuffers(2, pMesh->VBO);
    pMesh->VBO[0] = GL_UNDEF_OBJ;
    pMesh->VBO[1] = GL_UNDEF_OBJ;    
  };
  
  if (pMesh->VAO != GL_UNDEF_OBJ) {
    glDeleteVertexArrays(1, &(pMesh->VAO));    
    pMesh->VAO = GL_UNDEF_OBJ;
  };
  
  free(pMesh);
};

// adds a vertex to our buffer and returns the index in our vertice buffer
// return GL_UNDEF_OBJ if we couldn't allocate memory
GLuint meshAddVertex(mesh3d * pMesh, const vertex * pVertex) {
  if (pMesh == NULL) {
    return GL_UNDEF_OBJ;
  };
  
  if (pMesh->verticesData == NULL) {
    pMesh->numVertices = 0;
    pMesh->verticesSize = BUFFER_EXPAND;
    pMesh->verticesData = (vertex *) malloc(sizeof(vertex) * pMesh->verticesSize);
  } else if (pMesh->verticesSize <= pMesh->numVertices + 1) {
    pMesh->verticesSize += BUFFER_EXPAND;
    pMesh->verticesData = (vertex *) realloc(pMesh->verticesData, sizeof(vertex) * pMesh->verticesSize);
  };

  if (pMesh->verticesData == NULL) {
    // something bad must have happened
    meshErrCallback(1, "Couldn''t allocate vertex array data");
    pMesh->numVertices = 0;
    pMesh->verticesSize = 0;
    return GL_UNDEF_OBJ;
  } else {
    memcpy(&(pMesh->verticesData[pMesh->numVertices]), pVertex, sizeof(vertex));
    
    return pMesh->numVertices++; /* this will return our current value of numVertices and then increase it! */
  };
};

// adds a face (3 indices into vertex array)
// returns false on failure
bool meshAddFace(mesh3d * pMesh, GLuint pA, GLuint pB, GLuint pC) {
  if (pMesh == NULL) {
    return false;
  };
  
  if (pMesh->indicesData == NULL) {
    pMesh->numIndices = 0;
    pMesh->indicesSize = BUFFER_EXPAND;
    pMesh->indicesData = (GLuint *) malloc(sizeof(GLuint) * pMesh->indicesSize);
  } else if (pMesh->indicesSize <= pMesh->numIndices + 3) {
    pMesh->indicesSize += BUFFER_EXPAND;
    pMesh->indicesData = (GLuint *) realloc(pMesh->indicesData, sizeof(GLuint) * pMesh->indicesSize);
  };

  if (pMesh->indicesData == NULL) {
    // something bad must have happened
    meshErrCallback(2, "Couldn''t allocate index array data");    
    pMesh->numIndices = 0;
    pMesh->indicesSize = 0;
    return false;
  } else {
    pMesh->indicesData[pMesh->numIndices++] = pA;
    pMesh->indicesData[pMesh->numIndices++] = pB;
    pMesh->indicesData[pMesh->numIndices++] = pC;
    
    return true;
  }; 
};

// copies our vertex and index data to our GPU, creates/overwrites buffer objects as needed
// if pFreeBuffers is set to true our source data is freed up
// returns false on failure
bool meshCopyToGL(mesh3d * pMesh, bool pFreeBuffers) {
  if (pMesh == NULL) {
    return false;
  };
  
  // do we have data to load?
  if ((pMesh->numVertices == 0) || (pMesh->numIndices==0)) {
    meshErrCallback(3, "No data to copy to GL");    
    return false;
  };
  
  // make sure we have buffers
  if (pMesh->VBO[0] == GL_UNDEF_OBJ) {
    glGenVertexArrays(1, &(pMesh->VAO));
  };
  if (pMesh->VBO[0] == GL_UNDEF_OBJ) {
    glGenBuffers(2, pMesh->VBO);
  };
  
  // and load up our data
  // select our VAO
  glBindVertexArray(pMesh->VAO);
  
  // now load our vertices into our first VBO
  glBindBuffer(GL_ARRAY_BUFFER, pMesh->VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * pMesh->numVertices, pMesh->verticesData, GL_STATIC_DRAW);
  
  // now we need to configure our attributes, we use one for our position and one for our color attribute 
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) sizeof(vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) sizeof(vec3) + sizeof(vec3));
  
  // now we load our indices into our second VBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->VBO[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * pMesh->numIndices, pMesh->indicesData, GL_STATIC_DRAW);
  
  // at this point in time our two buffers are bound to our vertex array so any time we bind our vertex array
  // our two buffers are bound aswell

  // and clear our selected vertex array object
  glBindVertexArray(0);
  
  if (pFreeBuffers) {
    free(pMesh->verticesData);
//    pMesh->numVertices = 0; // we do not reset this because we wish to remember how many vertices we've loaded into GPU memory
    pMesh->verticesSize = 0;
    pMesh->verticesData = NULL;

    free(pMesh->indicesData);
//    pMesh->numIndices = 0; // we do not reset this because we wish to remember how many indices we've loaded into GPU memory
    pMesh->indicesSize = 0;
    pMesh->indicesData = NULL;    
  };
  
  return true;
};

// render our mesh
bool meshRender(mesh3d * pMesh) {
  if (pMesh == NULL) {
    return false;
  };
  
  if (pMesh->VAO == GL_UNDEF_OBJ) {
    meshErrCallback(4, "No VAO to render");    
    return false;
  } else if (pMesh->numIndices == 0) {
    meshErrCallback(5, "No data to render");    
    return false;
  };
  
  glBindVertexArray(pMesh->VAO);
  glDrawElements(GL_TRIANGLES, pMesh->numIndices, GL_UNSIGNED_INT, 0);	
  glBindVertexArray(0);
  
  return true;
};

//////////////////////////////////////////////////////////
//  Some nice useful primitives....

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


vertex cubeVertices[] = {
  // front
  -0.5,  0.5,  0.5,  0.0,  0.0,  1.0, 1.0 / 3.0,       0.0,          // vertex 0
   0.5,  0.5,  0.5,  0.0,  0.0,  1.0, 2.0 / 3.0,       0.0,          // vertex 1
   0.5, -0.5,  0.5,  0.0,  0.0,  1.0, 2.0 / 3.0, 1.0 / 4.0,          // vertex 2
  -0.5, -0.5,  0.5,  0.0,  0.0,  1.0, 1.0 / 3.0, 1.0 / 4.0,          // vertex 3

  // back
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0, 1.0 / 3.0, 1.0 / 2.0,          // vertex 4
  -0.5,  0.5, -0.5,  0.0,  0.0, -1.0, 2.0 / 3.0, 1.0 / 2.0,          // vertex 5
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0, 2.0 / 3.0, 3.0 / 4.0,          // vertex 6
   0.5, -0.5, -0.5,  0.0,  0.0, -1.0, 1.0 / 3.0, 3.0 / 4.0,          // vertex 7
   
   // left
  -0.5,  0.5, -0.5, -1.0,  0.0,  0.0, 1.0 / 3.0, 1.0 / 4.0,          // vertex 8  (5)
  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0, 2.0 / 3.0, 1.0 / 4.0,          // vertex 9  (0)
  -0.5, -0.5,  0.5, -1.0,  0.0,  0.0, 2.0 / 3.0, 2.0 / 4.0,          // vertex 10 (3)
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0, 1.0 / 3.0, 2.0 / 4.0,          // vertex 11 (6)

  // right
   0.5,  0.5,  0.5,  1.0,  0.0,  0.0, 1.0 / 3.0, 1.0 / 4.0,          // vertex 12 (1)
   0.5,  0.5, -0.5,  1.0,  0.0,  0.0, 2.0 / 3.0, 1.0 / 4.0,          // vertex 13 (4)
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0, 2.0 / 3.0, 2.0 / 4.0,          // vertex 14 (7)
   0.5, -0.5,  0.5,  1.0,  0.0,  0.0, 1.0 / 3.0, 2.0 / 4.0,          // vertex 15 (2)

  // top
  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0,       0.0,       0.0,          // vertex 16 (5)
   0.5,  0.5, -0.5,  0.0,  1.0,  0.0, 1.0 / 3.0,       0.0,          // vertex 17 (4)
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0, 1.0 / 3.0, 1.0 / 4.0,          // vertex 18 (1)
  -0.5,  0.5,  0.5,  0.0,  1.0,  0.0,       0.0, 1.0 / 4.0,          // vertex 19 (0)

  // bottom
  -0.5, -0.5,  0.5,  0.0, -1.0,  0.0, 2.0 / 3.0,       0.0,          // vertex 20 (3)
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0, 3.0 / 3.0,       0.0,          // vertex 21 (2)
   0.5, -0.5, -0.5,  0.0, -1.0,  0.0, 3.0 / 3.0, 1.0 / 4.0,          // vertex 22 (7)
  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0, 2.0 / 3.0, 1.0 / 4.0,          // vertex 23 (6)
};

// and now define our indices that make up our triangles
GLuint cubeIndices[] = {
  // front
   0,  1,  2,
   0,  2,  3,

  // back
   4,  5,  6, 
   4,  6,  7,  

  // left
   8,  9, 10,
   8, 10, 11,
  
  // right
  12, 13, 14,
  12, 14, 15,
  
  // top
  16, 17, 18,
  16, 18, 19,
  
  // bottom
  20, 21, 22,
  20, 22, 23,
};

#define CUBE_NUM_TRIANGLES (sizeof(cubeIndices) / sizeof(GLuint))

// loads a cube
bool meshMakeCube(mesh3d * pMesh, GLfloat pWidth, GLfloat pHeight, GLfloat pDepth) {
  if (pMesh == NULL) {
    return false;
  };
  
  // reset our buffers but reuse memory (if any)
  pMesh->numVertices = 0;
  pMesh->numIndices = 0;
  
  // add our vertices
  for (int i = 0; i < 24; i++) {
    vertex newvertex = cubeVertices[i];
    newvertex.V.x *= pWidth;
    newvertex.V.y *= pHeight;
    newvertex.V.z *= pDepth;
    meshAddVertex(pMesh, &newvertex);
  };
  
  // add our indices
  for (int i = 0; i < CUBE_NUM_TRIANGLES; i += 3) {
    meshAddFace(pMesh, cubeIndices[i], cubeIndices[i+1], cubeIndices[i+2]);
  };
  
  return true;
};

// loads a sphere
bool meshMakeSphere(mesh3d * pMesh, GLfloat pRadius) {
  int count = 0;

  if (pMesh == NULL) {
    return false;
  };
  
  // reset our buffers but reuse memory (if any)
  pMesh->numVertices = 0;
  pMesh->numIndices = 0;

  // slices of X every 7.5 degrees and yes we duplicate our first vertex (e.g. 145 points)
  for (GLfloat xAng = 0.0; xAng <= 360.0; xAng += 2.5) {
    // slices of Y every 15 degrees and yes we duplicate our first and last vertex for every slice (e.g. 73 points)
    for (GLfloat yAng = 0.0; yAng <= 180.0; yAng += 2.5) {
      vertex  myVertex;
      
      myVertex.V.x = pRadius * sin(angToRad(yAng)) * sin(angToRad(-xAng)) * 0.5;
      myVertex.V.y = pRadius * cos(angToRad(yAng)) * 0.5;
      myVertex.V.z = pRadius * -sin(angToRad(yAng)) * cos(angToRad(-xAng)) * 0.5;
      
      vec3Copy(&myVertex.N, &myVertex.V);
      vec3Normalise(&myVertex.N);
        
      myVertex.T.x = xAng / 360.0;
      myVertex.T.y = yAng / 180.0;
      
      meshAddVertex(pMesh, &myVertex);
      
      if ((xAng > 0.0) && (yAng > 0.0)) {
        meshAddFace(pMesh, count - 74, count - 1, count);
        meshAddFace(pMesh, count - 74, count, count - 73);
      };
      
      count++;
    };
  };

  
  return true;
};

#endif /* MESH_IMPLEMENTATION */

#endif /* !mesh3dh */
