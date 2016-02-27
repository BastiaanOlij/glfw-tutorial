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
 * functions. 
 *
 * Revision history:
 * 0.1  12-01-2016  First version with basic functions
 * 0.2  16-01-2016  Adding loading wavefront obj files
 * 0.3  08-02-2016  Added automatic load into GL memory
 *                  and passing a matrix to adjust loading
 *                  our wavefront file
 *
 ********************************************************/

#ifndef mesh3dh
#define mesh3dh

#include "errorlog.h"
#include "linkedlist.h"
#include "dynamicarray.h"
#include "varchar.h"
#include "math3d.h"
#include "material.h"

#define GL_UNDEF_OBJ      0xffffffff
#define BUFFER_EXPAND     100

// structure for our vertices
typedef struct vertex {
  vec3          V;                    // position of our vertice (XYZ)
  vec3          N;                    // normal of our vertice (XYZ)
  vec2          T;                    // texture coordinates (XY)
} vertex;

// structure for encapsulating mesh data
typedef struct mesh3d {
  unsigned int  retainCount;          // retain count for this object
  bool          visible;              // if true we render this
  char          name[50];             // name for this mesh 
  bool          canRender;            // if true we can render this
  bool          isLoaded;             // if true this is loaded into our GL memory
  
  // material to use
  material *    material;             // our material

  // mesh data
  dynarray *    vertices;             // our vertices
  dynarray *    indices;              // our indices
  
  // default model matrix
  mat4          defModel;             // our default model matrix

  // GPU state
  GLuint        VAO;                  // our vertex array object
  GLuint        VBO[2];               // our two vertex buffer objects
  GLuint        loadedIndices;        // number of indices loaded into GPU
} mesh3d;

#ifdef __cplusplus
extern "C" {
#endif

mesh3d * newMesh(GLuint pInitialVertices, GLuint pInitialIndices);
llist * newMeshList(void);
void meshRetain(mesh3d * pMesh);
void meshRelease(mesh3d * pMesh);
void meshSetMaterial(mesh3d * pMesh, material * pMat);
GLuint meshAddVertex(mesh3d * pMesh, const vertex * pVertex);
void meshFlipNormals(mesh3d * pMesh);
bool meshAddFace(mesh3d * pMesh, GLuint pA, GLuint pB, GLuint pC);
void meshFlipFaces(mesh3d * pMesh);
void meshCenter(mesh3d *pMesh);
bool meshCopyToGL(mesh3d * pMesh, bool pFreeBuffers);
bool meshRender(mesh3d * pMesh);

bool meshMakePlane(mesh3d * pMesh, int pHorzTiles, int pVertTiles, float pWidth, float pHeight);
bool meshMakeCube(mesh3d * pMesh, GLfloat pWidth, GLfloat pHeight, GLfloat pDepth, bool pFBLRTB);
bool meshMakeSphere(mesh3d * pMesh, GLfloat pRadius);

bool meshParseObj(const char * pData, llist * pAddToMeshList, llist * pMaterials, mat4 * pAdjust);

#ifdef __cplusplus
};
#endif  

#ifdef MESH_IMPLEMENTATION

// Initialize a new mesh that either has been allocated on the heap or allocated with
void meshInit(mesh3d * pMesh, GLuint pInitialVertices, GLuint pInitialIndices) {
  if (pMesh == NULL) {
    return;
  };
  
//  errorlog(0, "New mesh (%p)", pMesh);
  
  pMesh->retainCount = 1;
  pMesh->canRender = true;
  pMesh->isLoaded = false;
  pMesh->visible = true;
  strcpy(pMesh->name, "New");
  
  // init our material
  pMesh->material = NULL;

  // init our vertices
  if (pInitialVertices == 0) {
    pMesh->vertices = NULL;
  } else {
    pMesh->vertices = newDynArray(sizeof(vertex));
    if (pMesh->vertices == NULL) {
      errorlog(1, "Couldn''t allocate vertex array data");          
    } else if (!dynArrayCheckSize(pMesh->vertices, pInitialVertices)) {
      errorlog(1, "Couldn''t allocate vertex array data");      
    };
  };
  
  // init our indices
  if (pInitialIndices == 0) {
    pMesh->indices = NULL;
  } else {
    pMesh->indices = newDynArray(sizeof(GLuint));
    if (pMesh->indices == NULL) {
      errorlog(1, "Couldn''t allocate index array data");          
    } else if (!dynArrayCheckSize(pMesh->indices, pInitialIndices)) {
      errorlog(1, "Couldn''t allocate index array data");      
    };
  };
  
  mat4Identity(&(pMesh->defModel));
  
  pMesh->VAO = GL_UNDEF_OBJ;
  pMesh->VBO[0] = GL_UNDEF_OBJ;
  pMesh->VBO[1] = GL_UNDEF_OBJ;
  pMesh->loadedIndices = 0;
};

mesh3d * newMesh(GLuint pInitialVertices, GLuint pInitialIndices) {
  mesh3d * mesh = (mesh3d *) malloc(sizeof(mesh3d));
  if (mesh == NULL) {
    errorlog(1, "Couldn''t allocate memory for mesh");        
  } else {
    meshInit(mesh, pInitialVertices, pInitialIndices);
  };
  return mesh;
};

// list container for meshes
llist * newMeshList(void) {
  llist * meshList = newLlist((dataRetainFunc) meshRetain, (dataFreeFunc) meshRelease);
  return meshList;
};

// increase our retain count
void meshRetain(mesh3d * pMesh) {
  if (pMesh != NULL) {
    pMesh->retainCount++;

//    errorlog(0, "Increased retain count %p to %i", pMesh, pMesh->retainCount);
  };
};

// frees up data and buffers associated with this mesh if nothing retains it
void meshRelease(mesh3d * pMesh) {
  if (pMesh == NULL) {
    return;
  } else if (pMesh->retainCount > 1) {
    pMesh->retainCount--;

//    errorlog(0, "Decreased retain count %p to %i", pMesh, pMesh->retainCount);
    
    return;
  };
  
  if (pMesh->material != NULL) {
    matRelease(pMesh->material);
    pMesh->material = NULL;
  };
  
  if (pMesh->vertices != NULL) {
    dynArrayFree(pMesh->vertices);
    pMesh->vertices = NULL;
  };

  if (pMesh->indices != NULL) {
    dynArrayFree(pMesh->indices);
    pMesh->indices = NULL;
  };
  
  if (pMesh->VBO[0] != GL_UNDEF_OBJ) {
    // these are allocated in pairs so...
    glDeleteBuffers(2, pMesh->VBO);
    pMesh->VBO[0] = GL_UNDEF_OBJ;
    pMesh->VBO[1] = GL_UNDEF_OBJ;    
    pMesh->loadedIndices = 0;
  };
  
  if (pMesh->VAO != GL_UNDEF_OBJ) {
    glDeleteVertexArrays(1, &(pMesh->VAO));    
    pMesh->VAO = GL_UNDEF_OBJ;
  };
    
//  errorlog(0, "Freed mesh %s (%p)", pMesh->name, pMesh);
  free(pMesh);
};

// assigns our material
void meshSetMaterial(mesh3d * pMesh, material * pMat) {
  if (pMesh == NULL) {
    return;
  } else if (pMesh->material == pMat) {
    return;
  } else {
    if (pMesh->material != NULL) {
      matRelease(pMesh->material);
    };
    pMesh->material = pMat;
    if (pMesh->material != NULL) {
      matRetain(pMesh->material);
    };    
  };
};

// adds a vertex to our buffer and returns the index in our vertice buffer
// return GL_UNDEF_OBJ if we couldn't allocate memory
GLuint meshAddVertex(mesh3d * pMesh, const vertex * pVertex) {
  if (pMesh == NULL) {
    return GL_UNDEF_OBJ;
  };
  
  if (pMesh->vertices == NULL) {
    pMesh->vertices = newDynArray(sizeof(vertex));
    if (pMesh->vertices == NULL) {
      return GL_UNDEF_OBJ;
    };
  };

  dynArrayPush(pMesh->vertices, (void *) pVertex);
  pMesh->isLoaded = false;
  
  return pMesh->vertices->numEntries - 1;
};

// flips all normals of the mesh
void meshFlipNormals(mesh3d * pMesh) {
  int i;

  if (pMesh == NULL) {
    return;
  };

  for (i = 0; i < pMesh->vertices->numEntries; i++) {
    vertex * v = (vertex *) dynArrayDataAtIndex(pMesh->vertices, i);
    vec3Mult(&v->V, -1.0);
  };
};

// adds a face (3 indices into vertex array)
// returns false on failure
bool meshAddFace(mesh3d * pMesh, GLuint pA, GLuint pB, GLuint pC) {
  if (pMesh == NULL) {
    return false;
  };
  
  if (pMesh->indices == NULL) {
    pMesh->indices = newDynArray(sizeof(GLuint));
    if (pMesh->indices == NULL) {
      return GL_UNDEF_OBJ;
    };
  };
  
  dynArrayPush(pMesh->indices, &pA);
  dynArrayPush(pMesh->indices, &pB);
  dynArrayPush(pMesh->indices, &pC);
  pMesh->isLoaded = false;
  
  return true;
};

// flips all faces of the mesh
void meshFlipFaces(mesh3d * pMesh){
  int i;

  if (pMesh == NULL) {
    return;
  };

  for (i = 0; i < pMesh->indices->numEntries; i+=3) {
    GLuint * a = (GLuint *) dynArrayDataAtIndex(pMesh->indices, i);
    GLuint * c = (GLuint *) dynArrayDataAtIndex(pMesh->indices, i+2);
    GLuint swap = *a;
    *a = *c;
    *c = swap;
  };
};

// centers our mesh and adjust our model matrix accordingly
void meshCenter(mesh3d *pMesh) {
  if (pMesh == NULL) {
    // nothing much we can do here
  } else if (pMesh->vertices == NULL) {
    // can't do anything here
  } else if (pMesh->vertices->numEntries != 0) {
    vec3    center;
    int     i;

    // calculate our center
    vec3Set(&center, 0.0, 0.0, 0.0);
    for (i = 0; i < pMesh->vertices->numEntries; i++) {
      vec3 * vertice = dynArrayDataAtIndex(pMesh->vertices, i);

      vec3Add(&center, vertice);
    };
    vec3Div(&center, (float) pMesh->vertices->numEntries);

    // center our object
    for (i = 0; i < pMesh->vertices->numEntries; i++) {
      vec3 * vertice = dynArrayDataAtIndex(pMesh->vertices, i);

      vec3Sub(vertice, &center);
    };

    // adjust our matrix
    mat4Translate(&pMesh->defModel, &center);
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
  if ((pMesh->vertices == NULL) || (pMesh->indices==NULL)) {
    errorlog(3, "No data to copy to GL");
    pMesh->canRender = false;
    return false;
  } else if ((pMesh->vertices->numEntries == 0) || (pMesh->indices->numEntries == 0)) {
    errorlog(3, "No data to copy to GL");
    pMesh->canRender = false;
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
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * pMesh->vertices->numEntries, pMesh->vertices->data, GL_STATIC_DRAW);
  
  // now we need to configure our attributes, we use one for our position and one for our color attribute 
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) sizeof(vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *) (sizeof(vec3) + sizeof(vec3)));
  
  // now we load our indices into our second VBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->VBO[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * pMesh->indices->numEntries, pMesh->indices->data, GL_STATIC_DRAW);
  pMesh->loadedIndices = pMesh->indices->numEntries;
  
  // at this point in time our two buffers are bound to our vertex array so any time we bind our vertex array
  // our two buffers are bound aswell

  // and clear our selected vertex array object
  glBindVertexArray(0);
  
  if (pFreeBuffers) {
//    errorlog(0, "Free vertex array of %p (%p)", pMesh, pMesh->verticesData);

    dynArrayFree(pMesh->vertices);
    pMesh->vertices = NULL;
    
    dynArrayFree(pMesh->indices);
    pMesh->indices = NULL;
  };
  
  pMesh->canRender = true;
  pMesh->isLoaded = true;
  return true;
};

// render our mesh
bool meshRender(mesh3d * pMesh) {
  if (pMesh == NULL) {
    return false;
  } else if (pMesh->canRender == false) {
    // set this to false before, no need to double up on checking
    return false;
  };
  
  if (pMesh->isLoaded == false) {
    meshCopyToGL(pMesh, true); // if we want to keep our buffers, call this before we start rendering!
  };
  
  if (pMesh->VAO == GL_UNDEF_OBJ) {
    errorlog(4, "No VAO to render");
    pMesh->canRender = false; // prevent this error from appearing every frame  
    return false;
  } else if (pMesh->loadedIndices == 0) {
    errorlog(5, "No data to render");    
    pMesh->canRender = false; // prevent this error from appearing every frame  
    return false;
  };
  
  glBindVertexArray(pMesh->VAO);
  glDrawElements(GL_TRIANGLES, pMesh->loadedIndices, GL_UNSIGNED_INT, 0);	
  glBindVertexArray(0);
  
  return true;
};

//////////////////////////////////////////////////////////
//  Some nice useful primitives....

// We're building a plane:
//
// 0---1---2---3
// |\_ |\_ |\_ |
// |  \|  \|  \|
// 4---5---6---7
// |\_ |\_ |\_ |
// |  \|  \|  \|
// 8---9--10--11
// |\_ |\_ |\_ |
// |  \|  \|  \|
// 12-13--14--15

bool meshMakePlane(mesh3d * pMesh, int pHorzTiles, int pVertTiles, float pWidth, float pHeight) {
  int     x, y, idx = 0;
  float   posY = -pHeight / 2.0;
  float   sizeX = pWidth / pHorzTiles;
  float   sizeY = pHeight / pVertTiles;

  for (y = 0; y < pVertTiles+1; y++) {
    float   posX = -pWidth / 2.0;

    for (x = 0; x < pHorzTiles+1; x++) {
      // add our vertice
      vertex v;
      v.V.x = posX;
      v.V.y = 0.0;
      v.V.z = posY;
      v.N.x = 0.0;
      v.N.y = 1.0;
      v.N.z = 0.0;
      v.T.x = posX / pWidth;
      v.T.y = posY / pHeight;

      meshAddVertex(pMesh, &v);

      if ((x>0) && (y>0)) {
        // add triangles
        meshAddFace(pMesh, idx - pHorzTiles - 2, idx - pHorzTiles - 1, idx);
        meshAddFace(pMesh, idx - pHorzTiles - 2, idx, idx - 1);
      };

      posX += sizeX;
      idx++;
    };

    posY += sizeY;
  };

  return true;
};

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


vertex cubeVertices_Tshape[] = {
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

// texture layed out at FB - LR - TB (2 x 3 layout)
// note that we leave some space between the sides in the texture or we'll have trouble
// with our Linear interpolation
vertex cubeVertices_FBLRTB[] = {
  // front
  -0.5,  0.5,  0.5,  0.0,  0.0,  1.0,    2.0 / 1024.0,     2.0 / 1536.0,          // vertex 0
   0.5,  0.5,  0.5,  0.0,  0.0,  1.0,  510.0 / 1024.0,     2.0 / 1536.0,          // vertex 1
   0.5, -0.5,  0.5,  0.0,  0.0,  1.0,  510.0 / 1024.0,   510.0 / 1536.0,          // vertex 2
  -0.5, -0.5,  0.5,  0.0,  0.0,  1.0,    2.0 / 1024.0,   510.0 / 1536.0,          // vertex 3

  // back
   0.5,  0.5, -0.5,  0.0,  0.0, -1.0,  514.0 / 1024.0,     2.0 / 1536.0,          // vertex 4
  -0.5,  0.5, -0.5,  0.0,  0.0, -1.0, 1022.0 / 1024.0,     2.0 / 1536.0,          // vertex 5
  -0.5, -0.5, -0.5,  0.0,  0.0, -1.0, 1022.0 / 1024.0,   510.0 / 1536.0,          // vertex 6
   0.5, -0.5, -0.5,  0.0,  0.0, -1.0,  514.0 / 1024.0,   510.0 / 1536.0,          // vertex 7
   
   // left
  -0.5,  0.5, -0.5, -1.0,  0.0,  0.0,    2.0 / 1024.0,   514.0 / 1536.0,          // vertex 8  (5)
  -0.5,  0.5,  0.5, -1.0,  0.0,  0.0,  510.0 / 1024.0,   514.0 / 1536.0,          // vertex 9  (0)
  -0.5, -0.5,  0.5, -1.0,  0.0,  0.0,  510.0 / 1024.0,  1022.0 / 1536.0,          // vertex 10 (3)
  -0.5, -0.5, -0.5, -1.0,  0.0,  0.0,    2.0 / 1024.0,  1022.0 / 1536.0,          // vertex 11 (6)

  // right
   0.5,  0.5,  0.5,  1.0,  0.0,  0.0,  514.0 / 1024.0,   514.0 / 1536.0,          // vertex 12 (1)
   0.5,  0.5, -0.5,  1.0,  0.0,  0.0, 1022.0 / 1024.0,   514.0 / 1536.0,          // vertex 13 (4)
   0.5, -0.5, -0.5,  1.0,  0.0,  0.0, 1022.0 / 1024.0,  1022.0 / 1536.0,          // vertex 14 (7)
   0.5, -0.5,  0.5,  1.0,  0.0,  0.0,  514.0 / 1024.0,  1022.0 / 1536.0,          // vertex 15 (2)

  // top
  -0.5,  0.5, -0.5,  0.0,  1.0,  0.0,    2.0 / 1024.0,  1026.0 / 1536.0,          // vertex 16 (5)
   0.5,  0.5, -0.5,  0.0,  1.0,  0.0,  510.0 / 1024.0,  1026.0 / 1536.0,          // vertex 17 (4)
   0.5,  0.5,  0.5,  0.0,  1.0,  0.0,  510.0 / 1024.0,  1534.0 / 1536.0,          // vertex 18 (1)
  -0.5,  0.5,  0.5,  0.0,  1.0,  0.0,    2.0 / 1024.0,  1534.0 / 1536.0,          // vertex 19 (0)

  // bottom
  -0.5, -0.5,  0.5,  0.0, -1.0,  0.0,  514.0 / 1024.0,  1026.0 / 1536.0,          // vertex 20 (3)
   0.5, -0.5,  0.5,  0.0, -1.0,  0.0, 1022.0 / 1024.0,  1026.0 / 1536.0,          // vertex 21 (2)
   0.5, -0.5, -0.5,  0.0, -1.0,  0.0, 1022.0 / 1024.0,  1534.0 / 1536.0,          // vertex 22 (7)
  -0.5, -0.5, -0.5,  0.0, -1.0,  0.0,  514.0 / 1024.0,  1534.0 / 1536.0,          // vertex 23 (6)
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
bool meshMakeCube(mesh3d * pMesh, GLfloat pWidth, GLfloat pHeight, GLfloat pDepth, bool pFBLRTB) {
  int i;
  if (pMesh == NULL) {
    return false;
  };
  
  // reset our buffers but reuse memory (if any)
  if (pMesh->vertices != NULL) {
    pMesh->vertices->numEntries = 0;    
  };
  if (pMesh->indices != NULL) {
    pMesh->indices->numEntries = 0;
  };
  
  // add our vertices
  for (i = 0; i < 24; i++) {
    vertex newvertex = pFBLRTB ? cubeVertices_FBLRTB[i] : cubeVertices_Tshape[i];
    newvertex.V.x *= pWidth;
    newvertex.V.y *= pHeight;
    newvertex.V.z *= pDepth;
    meshAddVertex(pMesh, &newvertex);
  };
  
  // add our indices
  for (i = 0; i < CUBE_NUM_TRIANGLES; i += 3) {
    meshAddFace(pMesh, cubeIndices[i], cubeIndices[i+1], cubeIndices[i+2]);
  };
  
  return true;
};

// loads a sphere
bool meshMakeSphere(mesh3d * pMesh, GLfloat pRadius) {
  int vertices = 0, indices = 0;
  GLfloat xAng, yAng;

  if (pMesh == NULL) {
    return false;
  };
  
  // reset our buffers but reuse memory (if any)
  if (pMesh->vertices != NULL) {
    pMesh->vertices->numEntries = 0;    
  };
  if (pMesh->indices != NULL) {
    pMesh->indices->numEntries = 0;
  };

  // slices of X every 7.5 degrees and yes we duplicate our first vertex (e.g. 145 points)
  for (xAng = 0.0; xAng <= 360.0; xAng += 2.5) {
    // slices of Y every 15 degrees and yes we duplicate our first and last vertex for every slice (e.g. 73 points)
    for (yAng = 0.0; yAng <= 180.0; yAng += 2.5) {
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
        meshAddFace(pMesh, vertices - 74, vertices - 1, vertices);
        meshAddFace(pMesh, vertices - 74, vertices, vertices - 73);
        indices+=6;
      };
      
      vertices++;
    };
  };
  
//  errorlog(0, "Sphere has %i vertices and %i indices", vertices, indices);
  
  return true;
};

//////////////////////////////////////////////////////////
// parsers

// find a combination of vertex, normal and texture
typedef struct objVertexIdx {
  unsigned int  posIdx;             // index in our obj position buffer
  unsigned int  normalIdx;          // index in our obj normal buffer
  unsigned int  coordIdx;           // index in our obj texture coordinate buffer
  unsigned int  meshVertexIdx;      // resulting index in our mesh vertex buffer
} objVertexIdx;

// see if we have our vertex, else adds it, and returns the index
unsigned int objFindIndex(mesh3d * pMesh, varchar * pText, dynarray * pObjVertices, dynarray * pPos, dynarray * pNorm, dynarray * pCoords) {
  objVertexIdx    findVertex = { 0, 0, 0, 0 };
  vertex          newVertex;
  unsigned int    from = 0;
  unsigned int    pos;
  varchar *       idx;
  int             i;
  
  // add dividers
  varcharAppend(pText, "///", 3);
  
  // find position index
  pos = varcharPos(pText, "/", from);
  idx = varcharMid(pText, from, pos);
  if (idx == NULL) {
    // huh? really?! we must have a position!
    return 0;
  } else {
    sscanf(idx->text,"%i", &findVertex.posIdx);
    varcharRelease(idx);
    
    from = pos + 1;
    pos = varcharPos(pText, "/", from);
    idx = varcharMid(pText, from, pos);
    if (idx == NULL) {
      // this is allowed
      findVertex.coordIdx = 0;
    } else {
      sscanf(idx->text,"%i", &findVertex.coordIdx);
      varcharRelease(idx);      
    };
    
    from = pos + 1;
    pos = varcharPos(pText, "/", from);
    idx = varcharMid(pText, from, pos);
    if (idx == NULL) {
      // this is allowed
      findVertex.normalIdx = 0;
    } else {
      sscanf(idx->text,"%i", &findVertex.normalIdx);
      varcharRelease(idx);      
    };
  };

  // range check our indices (note they start at 1, 0 = undefined)
  if (findVertex.posIdx > pPos->numEntries) {
    errorlog(0, "Vertex out of bounds %i > %i", findVertex.posIdx, pPos->numEntries);
    return 0;
  };
  if (findVertex.coordIdx > pCoords->numEntries) {
    errorlog(0, "Texture coordinate out of bounds %i > %i", findVertex.coordIdx, pCoords->numEntries);
    findVertex.coordIdx = 0;
  };
  if (findVertex.normalIdx > pNorm->numEntries) {
    errorlog(0, "Normal out of bounds %i > %i", findVertex.normalIdx, pNorm->numEntries);
    findVertex.normalIdx = 0;    
  };
  
//  errorlog(0, "Parsed %i, %i, %i", findVertex.posIdx, findVertex.coordIdx, findVertex.normalIdx);
  
  // let's see if we already have it...
  for (i = 0; i < pObjVertices->numEntries; i++) {
    objVertexIdx compare;
    memcpy(&compare, dynArrayDataAtIndex(pObjVertices, i), sizeof(objVertexIdx));
    if ((compare.posIdx == findVertex.posIdx) && (compare.coordIdx == findVertex.coordIdx) && (compare.normalIdx == findVertex.normalIdx)) {
      return compare.meshVertexIdx;
    };
  };
  
  // setup our new vertex
  vec3Copy(&newVertex.V, (vec3 *)dynArrayDataAtIndex(pPos, findVertex.posIdx - 1));
  
  if (findVertex.coordIdx == 0) {
    newVertex.T.x = 0.0;
    newVertex.T.y = 0.0;
  } else {
    vec2Copy(&newVertex.T, (vec2 *)dynArrayDataAtIndex(pCoords, findVertex.coordIdx - 1));    
  };

  if (findVertex.normalIdx == 0) {
    vec3Copy(&newVertex.N, &newVertex.V);
    vec3Normalise(&newVertex.N);
  } else {
    vec3Copy(&newVertex.N, (vec3 *)dynArrayDataAtIndex(pNorm, findVertex.normalIdx - 1));    
  };
  
  findVertex.meshVertexIdx = meshAddVertex(pMesh, &newVertex);
  dynArrayPush(pObjVertices, &findVertex);
  
  return findVertex.meshVertexIdx;
};

// parse data loaded from a wavefront .obj file
// note that loading the dpDataata from disk should be implemented separately
// https://en.wikipedia.org/wiki/Wavefront_.obj_file
bool meshParseObj(const char * pData, llist * pAddToMeshList, llist * pMaterials, mat4 * pAdjust) {
  bool          finished = false;
  unsigned int  pos = 0;
  unsigned int  start = 0;
  unsigned int  len = 0;
  int           linecount = 1;
  dynarray *    positions = newDynArray(sizeof(vec3));
  dynarray *    normals = newDynArray(sizeof(vec3));
  dynarray *    coords = newDynArray(sizeof(vec2));
  mesh3d *      mesh = NULL;
  dynarray *    objVertices = NULL;
  mat3          normalMat;
  
  // this will give issues if a non uniform scale is used...
  mat3FromMat4(&normalMat, pAdjust);
  
  while (!finished) {
    if ((pData[pos] == 13) || (pData[pos] == 10) || (pData[pos] == 0)) {
      if (len > 0) {
        varchar * line = newVarchar();
        varcharAppend(line, pData + start, len);

        varcharTrim(line);
        
        if (line->text[0] == '#') {
          // skip, this is a comment
        } else {
          unsigned int pos;
        
          pos = varcharPos(line, " ", 0);
          if ((pos != VARCHAR_NOTFOUND) && (pos > 0)) {
            varchar * what = varcharMid(line, 0, pos);
            if (what == NULL) {
              errorlog(linecount, "Insufficient memory");
            } else {
              if (what->len == 0) {        
                errorlog(linecount, "Couldn't extract marker");        
              } else if (varcharCmp(what, "mtllib") == 0) {
                // load this material library, we're ignoring this, the material library should already be loaded...
              } else if (varcharCmp(what, "v") == 0) {
                // load a vertex
                vec3 vertex, adjvertex;
                sscanf(line->text + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
                
                mat4ApplyToVec3(&adjvertex, &vertex, pAdjust);
              
                dynArrayPush(positions, &adjvertex);
              } else if (varcharCmp(what, "vn") == 0) {
                // load a normal
                vec3 vertex, adjvertex;
                sscanf(line->text + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
              
                mat3ApplyToVec3(&adjvertex, &vertex, &normalMat);
                vec3Normalise(&adjvertex);
              
                dynArrayPush(normals, &adjvertex);
              } else if (varcharCmp(what, "vt") == 0) {
                // load a texture coordinate
                vec2 vertex;
                sscanf(line->text + 2, "%f %f", &vertex.x, &vertex.y);

                vertex.y = 1.0 - vertex.y;
              
                dynArrayPush(coords, &vertex);
              } else if (varcharCmp(what, "o") == 0) {
                // new object
                
                // first round of our old...
                if (objVertices != NULL) {
                  dynArrayFree(objVertices);
                  objVertices = NULL;
                };
                if (mesh != NULL) {
                  llistAddTo(pAddToMeshList, mesh);
                  meshRelease(mesh);
                  mesh = NULL;
                };
              
                // our new mesh
                mesh = newMesh(0, 0);
                strcpy(mesh->name, line->text + 2);
              
                // also need a new vertices index array
                objVertices = newDynArray(sizeof(objVertexIdx));
              } else if (varcharCmp(what, "g") == 0) {
                // new group
              
                // first round of our old...
                if (objVertices != NULL) {
                  dynArrayFree(objVertices);
                  objVertices = NULL;
                };
                if (mesh != NULL) {
                  llistAddTo(pAddToMeshList, mesh);
                  meshRelease(mesh);
                  mesh = NULL;
                };
              
                // our new mesh
                mesh = newMesh(0, 0);
                strcpy(mesh->name, line->text + 2);
              
                // also need a new vertices index array
                objVertices = newDynArray(sizeof(objVertexIdx));
              } else if (varcharCmp(what, "usemtl") == 0) {
                // use material
                if (pMaterials != NULL) {
                  // first we check if we need to create an object and then we start adding our face
                  if (mesh == NULL) {
                    mesh = newMesh(0, 0);
                    strcpy(mesh->name, "Default");                
                  };
                  if (objVertices == NULL) {
                    objVertices = newDynArray(sizeof(objVertexIdx));                
                  };
                  
                  // set our material, if it can't be found NULL is returned which is fine
                  meshSetMaterial(mesh, getMatByName(pMaterials, line->text + 7));
                };
              } else if (varcharCmp(what, "s") == 0) {
                // shininess, ignore for now
              } else if (varcharCmp(what, "f") == 0) {
                // face
                unsigned int from = pos + 1;
                unsigned int idx[3];
                int count = 0;
              
                // first we check if we need to create an object and then we start adding our face
                if (mesh == NULL) {
                  mesh = newMesh(0, 0);
                  strcpy(mesh->name, "Default");                
                };
                if (objVertices == NULL) {
                  objVertices = newDynArray(sizeof(objVertexIdx));                
                };
              
                // errorlog(linecount, "Testing '%s'", line->text);
              
                // append a space to our line so we can more easily process the line
                varcharAppend(line, " ", 1);

                // now process our face data...
                while ((pos = varcharPos(line, " ", from)) != VARCHAR_NOTFOUND) {
                  varchar * index = varcharMid(line, from, pos - from);
                  if (index == NULL) {
                    // ignore
                  } else if (index->len == 0) {
                    // ignore
                    varcharRelease(index);
                  } else {
                    // errorlog(linecount, "Found: '%s'", index->text);                
                    if (count < 2) {
                      idx[count] = objFindIndex(mesh, index, objVertices, positions, normals, coords);
                      count++;
                    } else {
                      idx[2] = objFindIndex(mesh, index, objVertices, positions, normals, coords);
                  
                      meshAddFace(mesh, idx[2], idx[1], idx[0]);
                      idx[1] = idx[2];
                    };
                  
                    varcharRelease(index);
                  };                
                
                  from = pos + 1;
                };
              } else {
                errorlog(linecount, "Unknown type %s", what->text);              
              };
          
              // no longer needed
              varcharRelease(what);            
            };
          } else {
            // for now we report what is unhandled
            errorlog(linecount, "Can't parse line %s", line->text);          
          };
        };
        
        varcharRelease(line);
      };
      
      if ((pData[pos] == 13) && (pData[pos+1] == 10)) {
        // window crlf, skip the lf
        pos++;
      };

      // next line...
      start = pos + 1;
      len = 0;
      linecount++;
    } else {
      len++;
    };
    
    if (pData[pos] == 0) {
      finished = true;
    } else {
      pos++;      
    };
  };
  
  // add our final mesh  
  if (objVertices != NULL) {
    dynArrayFree(objVertices);
    objVertices = NULL;
  };

  if (mesh != NULL) {
    llistAddTo(pAddToMeshList, mesh);
    meshRelease(mesh);
    mesh = NULL;
  };
  
  // free up our temporary data
  if (positions != NULL) {
    dynArrayFree(positions);
    positions = NULL;
  }
  if (normals != NULL) {
    dynArrayFree(normals);
    normals = NULL;    
  };
  if (coords != NULL) {
    dynArrayFree(coords);
    coords = NULL;    
  };
  
  errorlog(0, "Finished loading meshes");
  
  return true;
};


#endif /* MESH_IMPLEMENTATION */

#endif /* !mesh3dh */
