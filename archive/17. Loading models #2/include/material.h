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
 *
 ********************************************************/

#ifndef materialh
#define materialh

#define MAT_NO_TEXTURE 0xffffffff

// our libraries we need
#include "errorlog.h"
#include "stb/stb_image.h"
#include "math3d.h"

// structure for our material info
typedef struct material {
  unsigned int  retainCount;      // retain count for this object
  char          name[50];         // name of our material
  
  GLfloat       alpha;            // alpha for our material
  vec3          matColor;         // diffuse color for this material
  vec3          matSpecColor;     // specular color for this material
  GLfloat       shininess;        // shininess of this material
  GLuint        textureId;        // id of our texturemap
  GLuint        reflectId;        // id of our reflectionmap
} material;

#ifdef __cplusplus
extern "C" {
#endif

material * newMaterial(char * pName);
llist * newMatList(void);
void matRetain(material * pMat);
void matRelease(material * pMat);
material * getMatByName(llist * pMatList, char * pName);
bool matLoadTexture(material *pMat, char * pFileName, GLint pFilter, GLint pWrap);
bool matLoadReflect(material *pMat, char * pFileName);

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
    newMat->alpha = 1.0;
    vec3Set(&newMat->matColor, 1.0, 1.0, 1.0);
    vec3Set(&newMat->matSpecColor, 1.0, 1.0, 1.0);
    newMat->shininess = 50.0;
    newMat->textureId = MAT_NO_TEXTURE;
    newMat->reflectId = MAT_NO_TEXTURE;
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
    if (pMat->textureId != MAT_NO_TEXTURE) {
      glDeleteTextures(1, &pMat->textureId);
      pMat->textureId = MAT_NO_TEXTURE;
    };

    if (pMat->reflectId != MAT_NO_TEXTURE) {
      glDeleteTextures(1, &pMat->reflectId);
      pMat->reflectId = MAT_NO_TEXTURE;
    };
    
    // and free...
    free(pMat);
  };
};

// find a material by name in our list
// material * myMaterial = getMatByName(materials, "FunkyColor");
material * getMatByName(llist * pMatList, char * pName) {
  if (pMatList == NULL) {
    errorlog(-1, "Attempted to find material in NULL list");
    return NULL;
  };
  
  llistNode * node = pMatList->first;
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

// load image file for a texture
bool matLoadImage(GLuint * pTexture, char * pFileName, GLint pFilter, GLint pWrap) {
	int x, y, comp;
  unsigned char * data;

  // if we haven't got a texture yet, create it, else reuse it
  if (*pTexture == MAT_NO_TEXTURE) {
    glGenTextures(1, pTexture);
  };
  
	data = stbi_load(pFileName, &x, &y, &comp, 4);
  if (data == 0) {
    errorlog(-1, "Couldn't load %s", pFileName);
    return false;
  } else {
    errorlog(0, "Load %s", pFileName);
    
    glBindTexture(GL_TEXTURE_2D, *pTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, pWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, pWrap);  
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		stbi_image_free(data);
    
    return true;
  };  
};

// load our texture map
// matLoadTexture(myMaterial, "checkerboard.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
bool matLoadTexture(material *pMat, char * pFileName, GLint pFilter, GLint pWrap) {
  if (pMat == NULL) {
    errorlog(-1, "Attempted to load texture for NULL material");
    return false;
  };
  
  return matLoadImage(&pMat->textureId, pFileName, pFilter, pWrap);
};

// load our reflection map
// matLoadReflect(myMaterial, "chrome.png");
bool matLoadReflect(material *pMat, char * pFileName) {
  if (pMat == NULL) {
    errorlog(-1, "Attempted to load texture for NULL material");
    return false;
  };
  
  return matLoadImage(&pMat->reflectId, pFileName, GL_LINEAR, GL_CLAMP_TO_EDGE);
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
                  matLoadTexture(mat, line->text + 7, GL_LINEAR, GL_CLAMP_TO_EDGE);
                };                
              } else if (varcharCmp(what, "map_refl") == 0) {
                // our reflect texture map
                if (mat != NULL) {
                  matLoadReflect(mat, line->text + 9);
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
