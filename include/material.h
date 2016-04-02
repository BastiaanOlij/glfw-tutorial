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
#include "system.h"
#include "math3d.h"
#include "shaders.h"
#include "texturemap.h"

// and a structure to hold information about a light (temporarily moved here)
typedef struct lightSource {
  float             ambient;          // ambient factor for our light
  vec3              position;         // position of our light
  vec3              adjPosition;      // position of our light with view matrix applied
  texturemap *      shadowMap;        // shadowmap for this light
  mat4              shadowMat;        // view-projection matrix for this light
} lightSource;

// structure for our material info
typedef struct material {
  unsigned int      retainCount;      // retain count for this object
  char              name[50];         // name of our material
  bool              twoSided;         // is this a two sided material?
  
  shaderInfo *      matShader;        // shader to use for this material
  shaderInfo *      shadowShader;     // shader to use for rendering shadows
  
  GLfloat           alpha;            // alpha for our material
  vec3              matColor;         // diffuse color for this material
  vec3              matSpecColor;     // specular color for this material
  GLfloat           shininess;        // shininess of this material
  texturemap *      diffuseMap;       // id of our diffuse map
  texturemap *      reflectMap;       // id of our reflectionmap
  texturemap *      bumpMap;          // id of our bumpmap (normal map/height map)
} material;

#ifdef __cplusplus
extern "C" {
#endif

material * newMaterial(char * pName);
llist * newMatList(void);
void matRetain(material * pMat);
void matRelease(material * pMat);
material * getMatByName(llist * pMatList, char * pName);
void matSetShader(material * pMat, shaderInfo * pShader);
void matSetShadowShader(material * pMat, shaderInfo * pShader);
void matSetDiffuseMap(material * pMat, texturemap * pTMap);
void matSetReflectMap(material * pMat, texturemap * pTMap);
void matSetBumpMap(material * pMat, texturemap * pTMap);
bool matSelectProgram(material * pMat, shaderMatrices * pMatrices, lightSource * pLight);
bool matSelectShadow(material * pMat, shaderMatrices * pMatrices);

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
    newMat->matShader = NULL;
    newMat->shadowShader = NULL;
    newMat->twoSided = false;
    newMat->alpha = 1.0;
    vec3Set(&newMat->matColor, 1.0, 1.0, 1.0);
    vec3Set(&newMat->matSpecColor, 1.0, 1.0, 1.0);
    newMat->shininess = 50.0;
    newMat->diffuseMap = NULL;
    newMat->reflectMap = NULL;
    newMat->bumpMap = NULL;
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
    matSetBumpMap(pMat, NULL);

    // release shader
    matSetShader(pMat, NULL);
    matSetShadowShader(pMat, NULL);
    
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

// assign a different shader to our material
void matSetShader(material * pMat, shaderInfo * pShader) {
  if (pMat == NULL) {
    return;
  };

  // already set? nothing to do!
  if (pMat->matShader == pShader) {
    return;
  };

  // out with the old...
  if (pMat->matShader != NULL) {
     shaderRelease(pMat->matShader);
  };

  // in with the new
  pMat->matShader = pShader;
  if (pMat->matShader != NULL) {
     shaderRetain(pMat->matShader);
  };
};

// assign a different shadow shader to our material
void matSetShadowShader(material * pMat, shaderInfo * pShader) {
  if (pMat == NULL) {
    return;
  };

  // already set? nothing to do!
  if (pMat->shadowShader == pShader) {
    return;
  };

  // out with the old...
  if (pMat->shadowShader != NULL) {
     shaderRelease(pMat->shadowShader);
  };

  // in with the new
  pMat->shadowShader = pShader;
  if (pMat->shadowShader != NULL) {
     shaderRetain(pMat->shadowShader);
  };
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

void matSetBumpMap(material * pMat, texturemap * pTMap) {
  if (pMat == NULL) {
    return;
  };

  // already set? nothing to do!
  if (pMat->bumpMap == pTMap) {
    return;
  };

  // out with the old...
  if (pMat->bumpMap != NULL) {
     tmapRelease(pMat->bumpMap);
  };

  // in with the new
  pMat->bumpMap = pTMap;
  if (pMat->bumpMap != NULL) {
     tmapRetain(pMat->bumpMap);
  };
};

bool matSelectProgram(material * pMat, shaderMatrices * pMatrices, lightSource * pLight) {
  int     texture = 0;
  
  if (pMat == NULL) {
    errorlog(-1, "No material selected!");
    return false;
  } else if (pMat->matShader == NULL) {
    errorlog(-1, "No shader setup for this material!");
    return false;
  } else if (pMat->matShader->program == NO_SHADER) {
    errorlog(-1, "No shader compiled for this material!");
    return false;
  };

  if (pMat->twoSided) {
    glDisable(GL_CULL_FACE);  // disable culling
  } else {
    glEnable(GL_CULL_FACE);   // enable culling
  }

  glUseProgram(pMat->matShader->program);
  
  // setup camera info
  
  if (pMat->matShader->eyePosId >= 0) {
    vec3    tmpvector;
    shdMatGetEyePos(pMatrices, &tmpvector);
    glUniform3f(pMat->matShader->eyePosId, tmpvector.x, tmpvector.y, tmpvector.z);       
  };
  
  // setup our matrices
  
  if (pMat->matShader->projectionMatrixId >= 0) {
    glUniformMatrix4fv(pMat->matShader->projectionMatrixId, 1, false, (const GLfloat *) pMatrices->projection.m);
  };

  if (pMat->matShader->viewMatrixId >= 0) {
    glUniformMatrix4fv(pMat->matShader->viewMatrixId, 1, false, (const GLfloat *) pMatrices->view.m);
  };

  if (pMat->matShader->modelMatrixId >= 0) {
    glUniformMatrix4fv(pMat->matShader->modelMatrixId, 1, false, (const GLfloat *) pMatrices->model.m);
  };
  
  if (pMat->matShader->modelViewMatrixId >= 0) {
    glUniformMatrix4fv(pMat->matShader->modelViewMatrixId, 1, false, (const GLfloat *) shdMatGetModelView(pMatrices)->m);
  };

  // calculate the inverse of our model-view
  if (pMat->matShader->modelViewInverseId >= 0) {
    glUniformMatrix4fv(pMat->matShader->modelViewInverseId, 1, false, (const GLfloat *) shdMatGetInvModelView(pMatrices)->m);
  };
  
  // our normal matrix taken from our model matrix
  if (pMat->matShader->normalMatrixId >= 0) {
    glUniformMatrix3fv(pMat->matShader->normalMatrixId, 1, false, (const GLfloat *) shdMatGetNormal(pMatrices)->m);
  };

  // our normal matrix taken from our modelView matrix
  if (pMat->matShader->normalViewId >= 0) {
    glUniformMatrix3fv(pMat->matShader->normalViewId, 1, false, (const GLfloat *) shdMatGetNormalView(pMatrices)->m);
  };
  
  if (pMat->matShader->mvpId >= 0) {
    glUniformMatrix4fv(pMat->matShader->mvpId, 1, false, (const GLfloat *) shdMatGetMvp(pMatrices)->m);
  };
  
  // setup our light

  if (pMat->matShader->lightPosId >= 0) {
    glUniform3f(pMat->matShader->lightPosId, pLight->adjPosition.x, pLight->adjPosition.y, pLight->adjPosition.z);       
  };
  if (pMat->matShader->ambientId >= 0) {
    glUniform1f(pMat->matShader->ambientId, pLight->ambient);       
  };
  if (pMat->matShader->shadowMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pLight->shadowMap == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pLight->shadowMap->textureId);
    }
    glUniform1i(pMat->matShader->shadowMapId, texture); 
    texture++;   
  };
  if (pMat->matShader->shadowMatId >= 0) {
    glUniformMatrix4fv(pMat->matShader->shadowMatId, 1, false, (const GLfloat *) pLight->shadowMat.m);
  };

  // setup our material
  if (pMat->matShader->alphaId >= 0) {
    glUniform1f(pMat->matShader->alphaId, pMat->alpha);      
  };

  if (pMat->matShader->matColorId >= 0) {
    glUniform3f(pMat->matShader->matColorId, pMat->matColor.x, pMat->matColor.y, pMat->matColor.z);      
  };

  if (pMat->matShader->matSpecColorId >= 0) {
    glUniform3f(pMat->matShader->matSpecColorId, pMat->matSpecColor.x, pMat->matSpecColor.y, pMat->matSpecColor.z);      
  };
  
  if (pMat->matShader->shininessId >= 0) {
    glUniform1f(pMat->matShader->shininessId, pMat->shininess);      
  };

  if (pMat->matShader->textureMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat->diffuseMap == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->diffuseMap->textureId);      
    }
    glUniform1i(pMat->matShader->textureMapId, texture); 
    texture++;   
  };

  if (pMat->matShader->reflectMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat->reflectMap == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->reflectMap->textureId);      
    }
    glUniform1i(pMat->matShader->reflectMapId, texture); 
    texture++;   
  };  

  if (pMat->matShader->bumpMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat->bumpMap == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->bumpMap->textureId);      
    }
    glUniform1i(pMat->matShader->bumpMapId, texture); 
    texture++;   
  };  

  return true;
};

bool matSelectShadow(material * pMat, shaderMatrices * pMatrices) {
  int     texture = 0;
  
  if (pMat == NULL) {
    // ignore this, we don't cast shadows
    return false;
  } else if (pMat->shadowShader == NULL) {
    // ignore this, we don't cast shadows
    return false;
  } else if (pMat->shadowShader->program == NO_SHADER) {
    errorlog(-1, "No shadow shader compiled for this material!");
    return false;
  };

  if (pMat->twoSided) {
    glDisable(GL_CULL_FACE);  // disable culling
  } else {
    glEnable(GL_CULL_FACE);   // enable culling
  }

  glUseProgram(pMat->shadowShader->program);

  // we communicate very little to our shadow shaders
  if (pMat->shadowShader->mvpId >= 0) {
    glUniformMatrix4fv(pMat->shadowShader->mvpId, 1, false, (const GLfloat *) shdMatGetMvp(pMatrices)->m);
  };

  if (pMat->shadowShader->textureMapId >= 0) {
    glActiveTexture(GL_TEXTURE0 + texture);
    if (pMat->diffuseMap == NULL) {
      glBindTexture(GL_TEXTURE_2D, 0);      
    } else {
      glBindTexture(GL_TEXTURE_2D, pMat->diffuseMap->textureId);      
    }
    glUniform1i(pMat->shadowShader->textureMapId, texture); 
    texture++;   
  };

  return true;  
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
                  matSetDiffuseMap(mat, getTextureMapByFileName(line->text + 7, GL_LINEAR, GL_REPEAT, false));
                  tmapMakeMipMap(mat->diffuseMap);
                };                
              } else if (varcharCmp(what, "map_refl") == 0) {
                // our reflect texture map
                if (mat != NULL) {
                  // load our relection map, note that it will be retained inside of matSetRelectMap!;
                  matSetReflectMap(mat, getTextureMapByFileName(line->text + 9, GL_LINEAR, GL_CLAMP_TO_EDGE, false));
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
