/********************************************************
 * texturemaps.h - texturemap library by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define TEXTURE_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. 
 *
 * Revision history:
 * 0.1  07-02-2016  First version with basic functions
 *
 ********************************************************/

#ifndef texturemaph
#define texturemaph

// our libraries we need
#include <stb/stb_image.h>
#include "errorlog.h"
#include "linkedlist.h"

typedef struct texturemap {
  unsigned int      retainCount;      // retain count for this object
	char	            name[1024];       // (file) name for this texture
  GLint             filter;           // filter applied to texture object
  GLint             wrap;             // wrap applied to texture object
  GLuint            textureId;        // ID of our texture object
} texturemap;

#ifdef __cplusplus
extern "C" {
#endif

void tmapSetTexturePath(char * pPath);

texturemap * newTextureMap(const char * pName);
llist * newTMapList(void);
void tmapRetain(texturemap * pTMap);
void tmapRelease(texturemap * pTMap);
texturemap * getTextureMapByFileName(const char * pFileName, GLint pFilter, GLint pWrap);
bool tmapLoadImage(texturemap * pTMap, const char * pFileName, GLint pFilter, GLint pWrap);

void tmapReleaseCachedTextureMaps();

#ifdef __cplusplus
};
#endif  

#ifdef TEXTURE_IMPLEMENTATION

// some variables we maintain
llist * tmapTextureCache = NULL;
char tmapTexturePath[1024] = "";

// sets the locationf from which we load our texture maps
void tmapSetTexturePath(char * pPath) {
  strcpy(tmapTexturePath, pPath);
};

// create a new texture map object
texturemap * newTextureMap(const char * pName) {
  texturemap * newTmap = (texturemap *) malloc(sizeof(texturemap));
  if (newTmap != NULL) {
    newTmap->retainCount = 1;
    strcpy(newTmap->name, pName);
    glGenTextures(1, &(newTmap->textureId));

    // errorlog(0, "Created texture map %s", pName);
  };
  return newTmap;
};

llist * newTMapList(void) {
  llist * tmapList = newLlist((dataRetainFunc) tmapRetain, (dataFreeFunc) tmapRelease);
  return tmapList;
};

// increase the retain count on a texture map object
void tmapRetain(texturemap * pTMap) {
  if (pTMap == NULL) {
    errorlog(-1, "Attempted to retain NULL texturemap");
  } else {
    pTMap->retainCount++;    

    // errorlog(0, "Retained texture map %s => %i", pTMap->name, pTMap->retainCount);
  };
};

// decrease the retain count on a texture map object and free it up if it's no longer needed.
void tmapRelease(texturemap * pTMap) {
  if (pTMap == NULL) {
    errorlog(-1, "Attempted to release NULL texturemap");
    return;
  } else if (pTMap->retainCount > 1) {
    pTMap->retainCount--;
    
    // errorlog(0, "Released texture map %s => %i", pTMap->name, pTMap->retainCount);

    return;
  } else {
    glDeleteTextures(1, &(pTMap->textureId));

    // errorlog(0, "Released texture map %s => 0", pTMap->name);

    free(pTMap);
  };
};

// Finds if we have already loaded our texture map, if so we return it, if not we attempt to load it.
// Note that the calling method should retain the object!!
texturemap * getTextureMapByFileName(const char * pFileName, GLint pFilter, GLint pWrap) {
  texturemap * newTMap = NULL;

  if (tmapTextureCache == NULL) {
    // create a new linked list for our cache
    tmapTextureCache = newTMapList();
  } else {
    // first search our node to see if we already have our object
    llistNode * node = tmapTextureCache->first;
    while (node != NULL) {
      texturemap * tmap = (texturemap *) node->data;
      if ((strcmp(tmap->name,pFileName) == 0) && (tmap->filter == pFilter) && (tmap->wrap = pWrap)) {
        // already loaded :)
        // errorlog(0,"Found %s in cache", pFileName);
        return tmap;
      };
      node = node->next;
    };
  };

  // attempt to load it...
  newTMap = newTextureMap(pFileName);
  if (newTMap != NULL) {
    if (tmapLoadImage(newTMap, pFileName, pFilter, pWrap)) {
      // errorlog(0,"Adding %s to cache", pFileName);
      // add to our linked list
      llistAddTo(tmapTextureCache, newTMap);

      // now we can release it safely as it is still retained by our list.
      // if our calling object wishes to keep it around, it should retain it too.
      tmapRelease(newTMap);
    } else {
      // couldn't load it, no point in hanging on to it...
      tmapRelease(newTMap);
      newTMap = NULL;
    };
  };

  return newTMap;
};

// load image file for a texture
bool tmapLoadImage(texturemap * pTMap, const char * pFileName, GLint pFilter, GLint pWrap) {
  int x, y, comp;
  unsigned char * data;
  char fileName[1024];

  // if we haven't got a texture yet, create it, else reuse it
  if (pTMap == NULL) {
    return false;
  };
  
  sprintf(fileName, "%s%s",tmapTexturePath, pFileName);
  data = stbi_load(fileName, &x, &y, &comp, 4);
  if (data == 0) {
    errorlog(-1, "Couldn't load %s", fileName);
    return false;
  } else {
    errorlog(0, "Load %s", fileName);

    // remember these
    pTMap->filter = pFilter;
    pTMap->wrap = pWrap;
    
    glBindTexture(GL_TEXTURE_2D, pTMap->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, pWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, pWrap);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    
    return true;
  };  
};

// this function releases all texture maps retained in our texture cache. 
void tmapReleaseCachedTextureMaps() {
  if (tmapTextureCache != NULL) {
    llistFree(tmapTextureCache);
    tmapTextureCache = NULL;
  };
};

#endif /* TEXTURE_IMPLEMENTATION */

#endif /* !texturemaph */
