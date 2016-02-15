/********************************************************
 * material.h - material library by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define MATERIAL_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. 
 *
 * Revision history:
 * 0.1  17-01-2016  First version with basic functions
 * 0.2  06-02-2016  Added matSelectProgram and changed
 *                  texture mapping
 *
 ********************************************************/

#ifndef materialh
#define materialh

// our libraries we need
#include "errorlog.h"
#include "math3d.h"
#include "shaders.h"
#include "texturemap.h"

// structure for our material info
typedef struct material {
  unsigned int      retainCount;      // retain count for this object
  char              name[50];         // name of our material
  
  shaderStdInfo *   shader;           // shader to use for this material
  
  GLfloat           alpha;            // alpha for our material
  vec3              matColor;         // diffuse color for this material
  vec3              matSpecColor;     // specular color for this material
  GLfloat           shininess;        // shininess of this material
  texturemap *      diffuseMap;       // id of our diffuse map
  texturemap *      reflectMap;       // id of our reflectionmap
} material;

#ifdef __cplusplus
extern "C" {
#endif

material * newMaterial(char * pName);
llist * newMatList(void);
void matRetain(material * pMat);
void matRelease(material * pMat);
material * getMatByName(llist * pMatList, char * pName);
void matSetDiffuseMap(material * pMat, texturemap * pTMap);
void matSetReflectMap(material * pMat, texturemap * pTMap);
void matSelectProgram(material * pMat, shaderMatrices * pMatrices, lightSource * pLight);

bool matParseMtl(const char * pData, llist * pMaterials);

#ifdef __cplusplus
};
#endif  

#ifdef MATERIAL_IMPLEMENTATION

// creates a new material
// material * myMaterial = newMaterial("FunkyColor");
material * newMaterial(char * pName) {
  material * newMat = (material *) malloc(sizeof(material));
  if (newMat != NULL) {
    newMat->retainCount = 1;
    strcpy(newMat->name, pName);
    newMat->shader = NULL;
    newMat->alpha = 1.0;
    vec3Set(&newMat->matColor, 1.0, 1.0, 1.0);
    vec3Set(&newMat->matSpecColor, 1.0, 1.0, 1.0);
    newMat->shininess = 50.0;
    newMat->diffuseMap = NULL;
    newMat->reflectMap = NULL;
  };
  
  return newMat;
};

// list container for material
// llist * materials = newMatList()
llist * newMatList(void) {
  llist * matList = newLlist((dataRetainFunc) matRetain, (dataFreeFunc) matRelease);
  return matList;
};

// increases our materials retain count
// matRetain(myMaterial);
void matRetain(material * pMat) {
  if (pMat == NULL) {
    errorlog(-1, "Attempted to retain NULL material");
  } else {
    pMat->retainCount++;    
  };
};

// decreases the retain count on our material and deallocates it if the count reaches zero
// matRelease(myMaterial);
// myMaterial = NULL;
void matRelease(material * pMat) {
  if (pMat == NULL) {
    errorlog(-1, "Attempted to release NULL material");
    return;
  } else if (pMat->retainCount > 1) {
    pMat->retainCount--;
    
    return;
  } else {
    // clean up textures
    matSetDiffuseMap(pMat, NULL);
    matSetReflectMap(pMat, NULL);
    
    // and free...
    free(pMat);
  };
};

// find a material by name in our list
// material * myMaterial = getMatByName(materials, "FunkyColor");
material * getMatByName(llist * pMatList, char * pName) {
  llistNode * node;
  if (pMatList == NULL) {
    errorlog(-1, "Attempted to find material in NULL list");
    return NULL;
  };
  
  node = pMatList->first;
  while (node != NULL) {
    material * mat = node->data;
    if (mat != NULL) {
      if (strcmp(mat->name, pName) == 0) {
        return mat;
      };
    };
    node = node->next;
  };
  
  errorlog(-1,"Couldn't find material %s", pName);
  return NULL;
};

// assign a diffuse map to our material
void matSetDiffuseMap(material * pMat, texturemap * pTMap) {
  if (pMat == NULL) {
    return;
  };

  // already set? nothing to do!
  if (pMat->diffuseMap == pTMap) {
    return;
  };

  // out with the old...
  if (pMat->diffuseMap != NULL) {
     tmapRelease(pMat->diffuseMap);
  };

  // in with the new
  pMat->diffuseMap = pTMap;
  if (pMat->diffuseMap != NULL) {
     tmapRetain(pMat->diffuseMap);
  };
};

void matSetReflectMap(material * pMat, texturemap * pTMap) {
  if (pMat == NULL) {
    return;
  };

  // already set? nothing to do!
  if (pMat->reflectMap == pTMap) {
    return;
  };

  // out with the old...
  if (pMat->reflectMap != NULL) {
     tmapRelease(pMat->reflectMap);
  };

  // in with the new
  pMat->reflectMap = pTMap;
  if (pMat->reflectMap != NULL) {
     tmapRetain(pMat->reflectMap);
  };
};

void matSelectProgram(material * pMat, shaderMatrices * pMatrices, lightSource * pLight) {
  mat4  modelView, modelViewInverse, viewInverse, mvp;
  int   texture = 0;
  
  if (pMat->shader == NULL) {
    errorlog(-1, "No shader setup for this material!");
    return;
  } else if (pMat->shader->program == NO_SHADER) {
    errorlog(-1, "No shader compiled for this material!");
    return;
  };

  glUseProgram(pMat->shader->program);
  
  // setup camera info
  
  if (pMat->shader->eyePosId >= 0) {
    mat4Inverse(&viewInverse, &pMatrices->view);
    glUniform3f(pMat->shader->eyePosId, viewInverse.m[3][0], viewInverse.m[3][1], viewInverse.m[3][2]);       
  };
  
  // setup our matrices
  
  if (pMat->shader->projectionMatrixId >= 0) {
    glUniformMatrix4fv(pMat->shader->projectionMatrixId, 1, false, (const GLfloat *) pMatrices->projection.m);
  };

  if (pMat->shader->viewMatrixId >= 0) {
    glUniformMatrix4fv(pMat->shader->viewMatrixId, 1, false, (const GLfloat *) pMatrices->view.m);
  };

  if (pMat->shader->modelMatrixId >= 0) {
    glUniformMatrix4fv(pMat->shader->modelMatrixId, 1, false, (const GLfloat *) pMatrices->model.m);
  };
  
  mat4Copy(&modelView, &pMatrices->view);
  mat4Multiply(&modelView, &pMatrices->model);

  if (pMat->shader->modelViewMatrixId >= 0) {
    glUniformMatrix4fv(pMat->shader->modelViewMatrixId, 1, false, (const GLfloat *) modelView.m);
  };

  // calculate the inverse of our model-view
  mat4Inverse(&modelViewInverse, &modelView);
  if (pMat->shader->modelViewInverseId >= 0) {
    glUniformMatrix4fv(pMat->shader->modelViewInverseId, 1, false, (const GLfloat *) modelViewInverse.m);
  };
  
  // our normal matrix taken from our model matrix
  if (pMat->shader->normalMatrixId >= 0) {
    mat3  matrix;
    mat3FromMat4(&matrix, &pMatrices->model);
    glUniformMatrix3fv(pMat->shader->normalMatrixId, 1, false, (const GLfloat *) matrix.m);
  };

  // our normal matrix taken from our modelView matrix
  if (pMat->shader->normalViewId >= 0) {
    mat3  matrix;
    mat3FromMat4(&matrix, &modelView);
    glUniformMatrix3fv(pMat->shader->normalViewId, 1, false, (const GLfloat *) matrix.m);
  };

  mat4Copy(&mvp, &pMatrices->projection);
  mat4Multiply(&mvp, &modelView);
  
  if (pMat->shader->mvpId >= 0) {
    glUniformMatrix4fv(pMat->shader->mvpId, 1, false, (const GLfloat *) mvp.m);
  };
  
  // setup our light

  if (pMat->shader->lightPosId >= 0) {
    glUniform3f(pMat->shader->lightPosId, pLight->adjPosition.x, pLight->adjPosition.y, pLight->adjPosition.z);       
  };

  // setup our material
  if (pMat->shader->alphaId >= 0) {
    glUniform1f(pMat->shader->alphaId, pMat->alpha);      
  };

  if (pMat->shader->matColorId >= 0) {
    glUniform3f(pMat->shader->matColorId, pMat->matColor.x, pMat->matColor.y, pMat->matColor.z);      
  };

  if (pMat->shader->matSpecColorId >= 0) {
    glUniform3f(pMat->shader->matSpecColorId, pMat->matSpecColor.x, pMat->matSpecColor.y, pMat->matSpecColor.z);      
  };
  
  if (pMat->shader->shininessId >= 0) {
    glUniform1f(pMat->shader->shininessId, pMat->shininess);      
  };

  if (pMat->shader->textureMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat->diffuseMap == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->diffuseMap->textureId);      
    }
    glUniform1i(pMat->shader->textureMapId, texture); 
    texture++;   
  };

  if (pMat->shader->reflectMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat->reflectMap == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->reflectMap->textureId);      
    }
    glUniform1i(pMat->shader->reflectMapId, texture); 
    texture++;   
  };  
};

// parse data loaded from a wavefront .mtl file
// note that loading the data from disk should be implemented separately
// https://en.wikipedia.org/wiki/Wavefront_.obj_file
//
// matParseMtl(mtldata, materials);
bool matParseMtl(const char * pData, llist * pMaterials) {
  bool          finished = false;
  unsigned int  pos = 0;
  unsigned int  start = 0;
  unsigned int  len = 0;
  int           linecount = 1;
  material *    mat = NULL;

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
              } else if (varcharCmp(what, "newmtl") == 0) {
                // new material
                
                // first add the material we had
                if (mat != NULL) {
                  llistAddTo(pMaterials, mat);
                  matRelease(mat);
                  mat = NULL;
                };

                // create our new material
                mat = newMaterial(line->text + 7);
              } else if (varcharCmp(what, "illum") == 0) {
                // illumination model
                
                // 0. Color on and Ambient off
                // 1. Color on and Ambient on
                // 2. Highlight on
                // 3. Reflection on and Ray trace on
                // 4. Transparency: Glass on, Reflection: Ray trace on
                // 5. Reflection: Fresnel on and Ray trace on
                // 6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
                // 7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
                // 8. Reflection on and Ray trace off
                // 9. Transparency: Glass on, Reflection: Ray trace off
                // 10. Casts shadows onto invisible surfaces
                
                // we are ignoring this for now

              } else if (varcharCmp(what, "Ka") == 0) {
                // ambient color of our material
                
                // we are ignoring this, our lightsource defines our ambient color
              } else if (varcharCmp(what, "Kd") == 0) {
                // diffuse color of our material
                if (mat != NULL) {
                  sscanf(line->text + 3, "%f %f %f", &mat->matColor.x, &mat->matColor.y, &mat->matColor.z);                  
                };
              } else if (varcharCmp(what, "Ks") == 0) {
                // specular color of our material
                if (mat != NULL) {
                  sscanf(line->text + 3, "%f %f %f", &mat->matSpecColor.x, &mat->matSpecColor.y, &mat->matSpecColor.z);                  
                };
              } else if (varcharCmp(what, "Ke") == 0) {
                // not supported
              } else if (varcharCmp(what, "Ns") == 0) {
                // specular exponent of our material
                if (mat != NULL) {
                  sscanf(line->text + 3, "%f", &mat->shininess);                  
                };
              } else if (varcharCmp(what, "Ni") == 0) {
                // not supported
              } else if (varcharCmp(what, "d") == 0) {
                // dissolve, 0.0 is fully transparent, 1.0 is opaque
                if (mat != NULL) {
                  sscanf(line->text + 2, "%f", &mat->alpha);                  
                };
              } else if (varcharCmp(what, "Tr") == 0) {
                // transparent, 1.0 is fully transparent, 0.0 is opaque, should mirror d if both specified
                if (mat != NULL) {
                  GLfloat t;
                  sscanf(line->text + 3, "%f", &t);
                  mat->alpha = 1.0 - t;                  
                };
              } else if (varcharCmp(what, "Tf") == 0) {
                // not supported
              } else if (varcharCmp(what, "map_Kd") == 0) {
                // our diffuse texture map
                if (mat != NULL) {
                  // load our diffuse map, note that it will be retained inside of matSetDiffuseMap!;
                  matSetDiffuseMap(mat, getTextureMapByFileName(line->text + 7, GL_LINEAR, GL_REPEAT));
                };                
              } else if (varcharCmp(what, "map_refl") == 0) {
                // our reflect texture map
                if (mat != NULL) {
                  // load our relection map, note that it will be retained inside of matSetRelectMap!;
                  matSetReflectMap(mat, getTextureMapByFileName(line->text + 9, GL_LINEAR, GL_CLAMP_TO_EDGE));
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
  
  // add our last material
  if (mat != NULL) {
    llistAddTo(pMaterials, mat);
    matRelease(mat);
    mat = NULL;
  };
  
  return true;
};

#endif /* MATERIAL_IMPLEMENTATION */

#endif /* !materialh */
