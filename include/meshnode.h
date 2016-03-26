/********************************************************
 * meshnode.h - node library by Bastiaan Olij 2016
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
 * 0.1  08-02-2016  First version with basic functions
 * 0.2  13-02-2016  Added copy function
 *
 ********************************************************/

#ifndef meshnodeh
#define meshnodeh

#include "system.h"
#include "linkedlist.h"
#include "dynamicarray.h"
#include "varchar.h"
#include "math3d.h"
#include "material.h"
#include "mesh3d.h"

// structure for managing instances of mesh data
typedef struct meshNode {
  unsigned int  retainCount;          /* retain count for this object */
  bool          visible;              /* if true we render the mesh(es) contained within this node */
  char          name[250];            /* name for this node */
  
  // LOD limits
  float         maxDist;              /* maximum distance to camera */

  // our positioning matrix
  mat4          position;             /* position relative to our parent instance */
  
  // mesh to render
  mesh3d *      mesh;                 /* mesh to render, NULL is just a positioning node */

  // bounding volume
  mesh3d *      bounds;               /* if set, this is our bounding box that we check against */
  
  // children
  llist *       children;             /* child nodes */
  bool          firstVisOnly;         /* render the first visible child only (LOD) */
} meshNode;

#ifdef __cplusplus
extern "C" {
#endif

void meshNodeSetRenderBounds(bool pSet);
void meshNodeSetBoundsDebugMaterial(material * pBoundsMat);

meshNode * newMeshNode(const char * pName);
meshNode * newCopyMeshNode(const char *pName, meshNode * pCopy, bool pDeepCopy);
llist * newMeshNodeList(void);
void meshNodeRetain(meshNode * pNode);
void meshNodeRelease(meshNode * pNode);
void meshNodeSetMesh(meshNode * pNode, mesh3d * pMesh);
void meshNodeSetBounds(meshNode * pNode, mesh3d * pBounds);
void meshNodeMakeBounds(meshNode *pNode);
void meshNodeAddChild(meshNode * pNode, meshNode * pChild);
void meshNodeAddChildren(meshNode *pNode, llist * pMeshList);
void meshNodeRender(meshNode * pNode, shaderMatrices * pMatrices, material * pDefaultMaterial, lightSource * pSun);

#ifdef __cplusplus
};
#endif

#ifdef MESH_IMPLEMENTATION

bool mNrenderBounds = false;
material * mNboundsMaterial = NULL;

// enable/disable rendering our bounds
void meshNodeSetRenderBounds(bool pSet) {
  mNrenderBounds = pSet;
};

// sets the material we'll use for rendering our bounding volume for debugging
// note that we do not retain this and it must be set before we create our bounding volumes!
void meshNodeSetBoundsDebugMaterial(material * pBoundsMat) {
  mNboundsMaterial = pBoundsMat;
};

// create a new mesh node
meshNode * newMeshNode(const char * pName) {
  meshNode * newNode = (meshNode *) malloc(sizeof(meshNode));
  if (newNode != NULL) {
    newNode->retainCount = 1;
    newNode->visible = true;
    strcpy(newNode->name, pName);
    newNode->maxDist = 0;
    mat4Identity(&newNode->position);
    newNode->mesh = NULL;
    newNode->bounds = NULL;
    newNode->children = newMeshNodeList();
    newNode->firstVisOnly = false;
  };
  
  return newNode;
};

// copy a mesh node, if pDeepCopy is false we reference the same child nodes, if pDeepCopy is true we make copies of all the child nodes (not the meshes)
meshNode * newCopyMeshNode(const char *pName, meshNode * pCopy, bool pDeepCopy) {
  if (pCopy == NULL) {
    errorlog(-1, "Attempted to copy NULL node");
    return NULL;
  };

  meshNode * newNode = (meshNode *) malloc(sizeof(meshNode));
  if (newNode != NULL) {
    llistNode * lnode;
    newNode->retainCount = 1;
    newNode->visible = pCopy->visible;
    strcpy(newNode->name, pName);
    newNode->maxDist = pCopy->maxDist;
    mat4Copy(&newNode->position, &pCopy->position);
    newNode->mesh = NULL; /* start NULL! */
    meshNodeSetMesh(newNode, pCopy->mesh); /* now assign our mesh, note that we're thus retaining the same mesh as the node we're copying */
    newNode->bounds = NULL; /* start NULL! */
    meshNodeSetBounds(newNode, pCopy->bounds); /* now assign our bounds, note that we're thus retaining the same mesh as the node we're copying */
    newNode->children = newMeshNodeList();
    newNode->firstVisOnly = pCopy->firstVisOnly;

    // now copy our children
    lnode = pCopy->children->first;
    while (lnode != NULL) {
      meshNode * child = (meshNode *) lnode->data;

      // make a copy of our child
      if (pDeepCopy) {
        child = newCopyMeshNode(child->name, child, true);
      };

      // and add our child to our new node
      if (child != NULL) {
        meshNodeAddChild(newNode, child);
      };

      lnode = lnode->next;
    };
  };
  
  return newNode;
};

// create a linked list containing mesh nodes
llist * newMeshNodeList(void) {
  llist * nodeList = newLlist((dataRetainFunc) meshNodeRetain, (dataFreeFunc) meshNodeRelease);
  return nodeList;  
};

// retain a mesh node
void meshNodeRetain(meshNode * pNode) {
  if (pNode == NULL) {
    errorlog(-1, "Attempted to retain NULL node");
  } else {
    pNode->retainCount++;    
  };  
};

// release a mesh node and free it if it reaches a retain count of 0
void meshNodeRelease(meshNode * pNode) {
  if (pNode == NULL) {
    errorlog(-1, "Attempted to release NULL node");
    return;
  } else if (pNode->retainCount > 1) {
    pNode->retainCount--;
    
    return;
  } else {
    // free our mesh if its set
    meshNodeSetMesh(pNode, NULL);

    // free our bounds if its set
    meshNodeSetBounds(pNode, NULL);
    
    // free our children
    if (pNode->children != NULL) {
      llistFree(pNode->children);
      pNode->children = NULL;      
    };
    
    free(pNode);
  };
};

// set (or unset) our related mesh, the mesh will be release/retained as needed
void meshNodeSetMesh(meshNode * pNode, mesh3d * pMesh) {
  if (pNode == NULL) {
    errorlog(-1, "Attempted to set a mesh to a NULL node");
    return;
  } else if (pNode->mesh == pMesh) {
    return;
  } else {
    if (pNode->mesh != NULL) {
      meshRelease(pNode->mesh);
    };
    pNode->mesh = pMesh;
    if (pNode->mesh != NULL) {
      meshRetain(pNode->mesh);
    };    
  };  
};

// set (or unset) our related bounds, the mesh will be release/retained as needed
void meshNodeSetBounds(meshNode * pNode, mesh3d * pBounds) {
  if (pNode == NULL) {
    errorlog(-1, "Attempted to set bounds for a NULL node");
    return;
  } else if (pNode->bounds == pBounds) {
    return;
  } else {
    if (pNode->bounds != NULL) {
      meshRelease(pNode->bounds);
    };
    pNode->bounds = pBounds;
    if (pNode->bounds != NULL) {
      meshRetain(pNode->bounds);
    };    
  };  
};

void meshNodeGetMinMax(meshNode *pNode, vec3 * pMin, vec3 * pMax, const mat4 * pModel) {
  llistNode * child;
  int i;

  if (pNode == NULL) {
    return; // ignore
  };

  if (pNode->bounds != NULL) {
    // this object already has bounds;)
    if (pNode->bounds->vertices != NULL) {
      for (i = 0; i < pNode->bounds->vertices->numEntries; i++) {
        vec3 vertice;
        vec3 * orgVertice = dynArrayDataAtIndex(pNode->bounds->vertices, i);

        mat4ApplyToVec3(&vertice, orgVertice, pModel);

        if (pMin->x > vertice.x) { pMin->x = vertice.x; };
        if (pMin->y > vertice.y) { pMin->y = vertice.y; };
        if (pMin->z > vertice.z) { pMin->z = vertice.z; };

        if (pMax->x < vertice.x) { pMax->x = vertice.x; };
        if (pMax->y < vertice.y) { pMax->y = vertice.y; };
        if (pMax->z < vertice.z) { pMax->z = vertice.z; };
      };
    };
  } else {
    // check our mesh and any child nodes
    if (pNode->mesh != NULL) {
      if (pNode->mesh->vertices != NULL) {
        for (i = 0; i < pNode->mesh->vertices->numEntries; i++) {
          vec3 vertice;
          vec3 * orgVertice = dynArrayDataAtIndex(pNode->mesh->vertices, i);

          mat4ApplyToVec3(&vertice, orgVertice, pModel);

          if (pMin->x > vertice.x) { pMin->x = vertice.x; };
          if (pMin->y > vertice.y) { pMin->y = vertice.y; };
          if (pMin->z > vertice.z) { pMin->z = vertice.z; };

          if (pMax->x < vertice.x) { pMax->x = vertice.x; };
          if (pMax->y < vertice.y) { pMax->y = vertice.y; };
          if (pMax->z < vertice.z) { pMax->z = vertice.z; };
        };
      };
    };

    if (pNode->children != NULL) {
      llistNode * node = pNode->children->first;
      
      while (node != NULL) {
        meshNode * childNode = (meshNode *)node->data;
        mat4 model; 

        // make our model matrix
        mat4Copy(&model, pModel);
        mat4Multiply(&model, &childNode->position);

        meshNodeGetMinMax(childNode, pMin, pMax, &model);

        node = node->next;
      };
    };
  };
};

// generate a bounds mesh (for now just cubes)
void meshNodeMakeBounds(meshNode *pNode) {
  vec3 minVec, maxVec, tmpVec;
  mesh3d * mesh;
  mat4 model;

  // determine our bounds
  vec3Set(&minVec, 0.0, 0.0, 0.0);
  vec3Set(&maxVec, 0.0, 0.0, 0.0);
  mat4Identity(&model);
  meshNodeGetMinMax(pNode, &minVec, &maxVec, &model);

  // create a cube from this
  mesh = newMesh(8, 12);
  meshSetMaterial(mesh, mNboundsMaterial);

  // add our vertices and faces, we're going minimalistic here as we don't really care about normals or texturing
  meshAddVNT(mesh, vec3Set(&tmpVec, minVec.x, minVec.y, minVec.z), NULL, NULL); // 0
  meshAddVNT(mesh, vec3Set(&tmpVec, maxVec.x, minVec.y, minVec.z), NULL, NULL); // 1
  meshAddVNT(mesh, vec3Set(&tmpVec, maxVec.x, maxVec.y, minVec.z), NULL, NULL); // 2
  meshAddVNT(mesh, vec3Set(&tmpVec, minVec.x, maxVec.y, minVec.z), NULL, NULL); // 3

  meshAddVNT(mesh, vec3Set(&tmpVec, maxVec.x, minVec.y, maxVec.z), NULL, NULL); // 4
  meshAddVNT(mesh, vec3Set(&tmpVec, minVec.x, minVec.y, maxVec.z), NULL, NULL); // 5
  meshAddVNT(mesh, vec3Set(&tmpVec, minVec.x, maxVec.y, maxVec.z), NULL, NULL); // 6
  meshAddVNT(mesh, vec3Set(&tmpVec, maxVec.x, maxVec.y, maxVec.z), NULL, NULL); // 7

  meshAddFace(mesh, 0, 1, 2); // front
  meshAddFace(mesh, 0, 2, 3);
  meshAddFace(mesh, 4, 5, 6); // back
  meshAddFace(mesh, 4, 6, 7);

  meshAddFace(mesh, 0, 5, 4); // bottom
  meshAddFace(mesh, 1, 0, 4);
  meshAddFace(mesh, 2, 7, 6); // top
  meshAddFace(mesh, 3, 2, 6);

  meshAddFace(mesh, 6, 5, 0); // left
  meshAddFace(mesh, 3, 6, 0);
  meshAddFace(mesh, 1, 4, 7); // right
  meshAddFace(mesh, 1, 7, 2);

  meshNodeSetBounds(pNode, mesh);
};

// add a child node to our node
void meshNodeAddChild(meshNode * pNode, meshNode * pChild) {
  if (pNode == NULL) {
    errorlog(-1, "Attempted to add a child node to a NULL node");
    return;
  } else if (pChild == NULL) {
    errorlog(-1, "Attempted to add a NULL node");
    return;
  } else {
    llistAddTo(pNode->children, pChild);
  };
};

// add children to our node for all the meshes contained in the mesh list
void meshNodeAddChildren(meshNode *pNode, llist * pMeshList) {
  if (pNode == NULL) {
    errorlog(-1, "Attempted to add meshes to NULL node");
    return;
  } else if (pMeshList == NULL) {
    errorlog(-1, "No meshes to add to node");
    return;
  } else {
    llistNode * node = pMeshList->first;
    while (node != NULL) {
      mesh3d * mesh = (mesh3d *) node->data;
      meshNode * newNode = newMeshNode(mesh->name);
      
      mat4Copy(&newNode->position, &mesh->defModel);
      meshNodeSetMesh(newNode, mesh);
      
      meshNodeAddChild(pNode, newNode);
      meshNodeRelease(newNode);   // now retained by our children linked list
      
      node = node->next;
    };
  };
};

// structure for storing info about transparent meshes that we render later on
typedef struct renderMesh {
  mesh3d *  mesh;
  mat4      model;
  GLfloat   z;
} renderMesh;

// build our no-alpha and alpha render lists based on the contents of our node
bool meshNodeBuildRenderList(const meshNode * pNode, const mat4 * pModel, shaderMatrices * pMatrices, dynarray * pNoAlpha, dynarray * pAlpha) {
  mat4 model;
  
  // is there anything to do?
  if (pNode == NULL) {
    return false;
  } else if (pNode->visible == false) {
    return false;
  };
  
  // make our model matrix
  mat4Copy(&model, pModel);
  mat4Multiply(&model, &pNode->position);

  // check our distance
  if (pNode->maxDist > 0) {
    float distance;
    vec3  pos, eye;

    // first get our position from our model matrix
    vec3Set(&pos, model.m[3][0], model.m[3][1], model.m[3][2]);

    // then get our eye position
    shdMatGetEyePos(pMatrices, &eye);


    // subtract our camera position to get our relative position
    vec3Sub(&pos, &eye);

    // and get our distance
    distance = vec3Lenght(&pos);

    if (distance > pNode->maxDist) {
      return false;
    };
  };
  
  if (pNode->bounds != NULL) {
    mat4 mvp;

    mat4Copy(&mvp, shdMatGetViewProjection(pMatrices));
    mat4Multiply(&mvp, &model);
    if (meshTestVolume(pNode->bounds, &mvp) == false) {
      // yes we're not rendering but we did pass our LOD test so we're done...
      return true;
    };

    if (mNrenderBounds) {
      renderMesh render;

      // make sure we don't loose our buffers
      if (pNode->bounds->isLoaded == false) {
        meshCopyToGL(pNode->bounds, false);
      };

      // add our mesh
      render.mesh = pNode->bounds;
      mat4Copy(&render.model, &model);
      render.z = 0.0; // not yet used, need to apply view matrix to calculate

      dynArrayPush(pAlpha, &render); // this copies our structure
    };
  };

  if (pNode->mesh != NULL) {
    renderMesh render;

    if (pNode->mesh->visible == false) {
      return false;
    };

    // add our mesh
    render.mesh = pNode->mesh;
    mat4Copy(&render.model, &model);
    render.z = 0.0; // not yet used, need to apply view matrix to calculate
    
    if (pNode->mesh->material == NULL) {
      dynArrayPush(pNoAlpha, &render); // this copies our structure
    } else if (pNode->mesh->material->alpha != 1.0) {
      dynArrayPush(pAlpha, &render); // this copies our structure      
    } else {
      dynArrayPush(pNoAlpha, &render); // this copies our structure      
    };
  };
  
  if (pNode->children != NULL) {
    llistNode * node = pNode->children->first;
    
    while (node != NULL) {
      bool visible = meshNodeBuildRenderList((meshNode *) node->data, &model, pMatrices, pNoAlpha, pAlpha);

      if (pNode->firstVisOnly && visible) {
        // we've rendered our first visible child, ignore the rest!
        node = NULL;
      } else {
        node = node->next;
      };
    };
  };

  return true;
};

// render the contents of our node to the current output
void meshNodeRender(meshNode * pNode, shaderMatrices * pMatrices, material * pDefaultMaterial, lightSource * pSun) {
  dynarray *      meshesWithoutAlpha  = newDynArray(sizeof(renderMesh));
  dynarray *      meshesWithAlpha     = newDynArray(sizeof(renderMesh));
  mat4            model;
  int             i;

  // prepare our array with things to render....
  mat4Identity(&model);
  meshNodeBuildRenderList(pNode, &model, pMatrices, meshesWithoutAlpha, meshesWithAlpha);
  
  // now render no-alpha
  glDisable(GL_BLEND);

  for (i = 0; i < meshesWithoutAlpha->numEntries; i++) {
    bool selected = true;
    renderMesh * render = dynArrayDataAtIndex(meshesWithoutAlpha, i);
  
    shdMatSetModel(pMatrices, &render->model);
    if (render->mesh->material == NULL) {
      selected = matSelectProgram(pDefaultMaterial, pMatrices, pSun);
    } else {
      selected = matSelectProgram(render->mesh->material, pMatrices, pSun);
    };

    if (selected) {
      meshRender(render->mesh);
    } else {
      // couldn't select our material? don't attemp again
      render->mesh->visible = false;
    };
  };    
  
  
  // and render alpha
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  for (i = 0; i < meshesWithAlpha->numEntries; i++) {
    bool selected = true;
    renderMesh * render = dynArrayDataAtIndex(meshesWithAlpha, i);
  
    shdMatSetModel(pMatrices, &render->model);
    selected = matSelectProgram(render->mesh->material, pMatrices, pSun);

    if (selected) {
      meshRender(render->mesh);
    } else {
      // couldn't select our material? don't attemp again
      render->mesh->visible = false;
    };
  };
  
  dynArrayFree(meshesWithAlpha);
  dynArrayFree(meshesWithoutAlpha);
};

#endif /* MESH_IMPLEMENTATION */

#endif /* !meshnodeh */