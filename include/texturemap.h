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
#include "system.h"
#include "linkedlist.h"

typedef struct texturemap {
  unsigned int      retainCount;      // retain count for this object
	char	            name[1024];       // (file) name for this texture
  int               width;            // width of this image
  int               height;           // height of this image
  unsigned char *   data;             // raw texture data
  GLint             filter;           // filter applied to texture object
  GLint             wrap;             // wrap applied to texture object
  GLuint            textureId;        // ID of our texture object
  GLuint            depthBufferId;    // ID of our depth texture (if we needed one)
  GLuint            frameBufferId;    // ID of our framebuffer for render to texture
} texturemap;

#ifdef __cplusplus
extern "C" {
#endif

void tmapSetTexturePath(char * pPath);

texturemap * newTextureMap(const char * pName);
llist * newTMapList(void);
void tmapRetain(texturemap * pTMap);
void tmapRelease(texturemap * pTMap);
texturemap * getTextureMapByFileName(const char * pFileName, GLint pFilter, GLint pWrap, bool pKeepData);
bool tmapLoadImage(texturemap * pTMap, const char * pFileName, GLint pFilter, GLint pWrap, bool pKeepData);
bool tmapMakeMipMap(texturemap * pTMap);
bool tmapLoadData(texturemap * pTMap, const unsigned char * pData, int pWidth, int pHeight, GLint pFilter, GLint pWrap, GLint pIntFormat, GLint pFormat, GLint pType);
vec4 tmapGetPixel(texturemap * pTMap, float pS, float pT);
bool tmapRenderToTexture(texturemap * pTMap, bool pNeedDepthBuffer);
void tmapFreeFrameBuffers(texturemap * pTMap);

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
    newTmap->width = 0;
    newTmap->height = 0;
    newTmap->data = NULL;
    newTmap->filter = 0;
    newTmap->wrap = 0;
    glGenTextures(1, &(newTmap->textureId));
    newTmap->frameBufferId = 0;
    newTmap->depthBufferId = 0;

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
    tmapFreeFrameBuffers(pTMap);

    glDeleteTextures(1, &pTMap->textureId);

    if (pTMap->data != NULL) {
      stbi_image_free(pTMap->data);
      pTMap->data = NULL;
    };

    // errorlog(0, "Released texture map %s => 0", pTMap->name);

    free(pTMap);
  };
};

// Finds if we have already loaded our texture map, if so we return it, if not we attempt to load it.
// Note that the calling method should retain the object!!
texturemap * getTextureMapByFileName(const char * pFileName, GLint pFilter, GLint pWrap, bool pKeepData) {
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
    if (tmapLoadImage(newTMap, pFileName, pFilter, pWrap, pKeepData)) {
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
bool tmapLoadImage(texturemap * pTMap, const char * pFileName, GLint pFilter, GLint pWrap, bool pKeepData) {
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
    pTMap->width = x;
    pTMap->height = y;
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pTMap->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, pWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, pWrap);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    if (pKeepData) {
      pTMap->data = data;
    } else {
      stbi_image_free(data);      
    };
    
    return true;
  };  
};

// initializes our texture using a datasource.
// note that if pData is empty we'll end up with an empty texture
bool tmapLoadData(texturemap * pTMap, const unsigned char * pData, int pWidth, int pHeight, GLint pFilter, GLint pWrap, GLint pIntFormat, GLint pFormat, GLint pType) {
  // if we haven't got a texture yet, create it, else reuse it
  if (pTMap == NULL) {
    return false;
  };

  // remember these
  pTMap->filter = pFilter;
  pTMap->wrap = pWrap;
  pTMap->width = pWidth;
  pTMap->height = pHeight;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pTMap->textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, pWrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, pWrap);
  glTexImage2D(GL_TEXTURE_2D, 0, pIntFormat, pWidth, pHeight, 0, pFormat, pType, pData);    

  return true;
};

// generates mipmaps for our texture
bool tmapMakeMipMap(texturemap * pTMap) {
  // if we haven't got a texture yet, create it, else reuse it
  if (pTMap == NULL) {
    return false;
  };

  // make sure we actually use our mimpmapping..
  if (pTMap->filter == GL_NEAREST) {
    pTMap->filter = GL_NEAREST_MIPMAP_NEAREST;
  } else if (pTMap->filter == GL_LINEAR) {
    pTMap->filter = GL_LINEAR_MIPMAP_LINEAR;
  };

  // generate mipmap, we assume image has been loaded
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pTMap->textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pTMap->filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pTMap->filter);
  glGenerateMipmap(GL_TEXTURE_2D);

  return true;
};

vec4 tmapGetPixelXY(texturemap * pTMap, int pX, int pY) {
  vec4 pixel;
  int pos;

  // tile 
  pX = pX % pTMap->width;
  pY = pY % pTMap->height;

  // adjust just in case..
  if (pX < 0.0) {
    pX += pTMap->width;
  };
  if (pY < 0.0) {
    pY += pTMap->height;
  };

  pos = (pX + (pY * pTMap->width)) * 4;
  pixel.x = (float) pTMap->data[pos];
  pixel.y = (float) pTMap->data[pos+1];
  pixel.z = (float) pTMap->data[pos+2];
  pixel.w = (float) pTMap->data[pos+3];
  vec4Div(&pixel, 255.0);

  return pixel;
};

vec4 tmapGetPixel(texturemap * pTMap, float pS, float pT) {
  vec4 pixel;
  vec4Set(&pixel, 1.0, 1.0, 1.0, 1.0);
  if (pTMap == NULL) {
    // huh?
    errorlog(-1, "No map specified");
  } else if (pTMap->data == NULL) {
    // double huh?
    errorlog(-1, "No mapdata available");
  } else {
    vec4 p1, p2, p3, p4;
    float xf = (pS * pTMap->width);
    float yf = (pT * pTMap->height);
    int x, y;

    // floor our values
    x = floor(xf);
    y = floor(yf);

    // get our deltas
    xf = xf - x;
    yf = yf - y;

    // get our 4 surrounding pixels
    p1 = tmapGetPixelXY(pTMap, x, y);
    p2 = tmapGetPixelXY(pTMap, x+1, y);
    p3 = tmapGetPixelXY(pTMap, x, y+1);
    p4 = tmapGetPixelXY(pTMap, x+1, y+1);

    // calculate the horizontal deltas
    vec4Sub(&p2, &p1);
    vec4Sub(&p4, &p3);

    // interpolate horizontally
    vec4Mult(&p2, xf);
    vec4Mult(&p4, xf);

    // add back in
    vec4Add(&p1, &p2);
    vec4Add(&p3, &p4);

    // calculate the vertical delta
    vec4Sub(&p3, &p1);

    // interpolate vertically
    vec4Mult(&p3, yf);

    // add back in
    vec4Add(&p1, &p3);

    // and copy to pixel
    vec4Copy(&pixel, &p1);
  };

  return pixel;
};

// Prepare our texture so we can render to it. 
// if needed a new framebuffer will be created and made current.
// the calling routine will be responsible for unbinding the framebuffer.
bool tmapRenderToTexture(texturemap * pTMap, bool pNeedDepthBuffer) {
  if (pTMap == NULL) {
    return false;
  };

  // create our frame buffer if we haven't already
  if (pTMap->frameBufferId == 0) {
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    GLenum status;

    glGenFramebuffers(1, &pTMap->frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, pTMap->frameBufferId);

    // bind our texture map
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTMap->textureId, 0);

    // init and bind our depth buffer
    if ((pTMap->depthBufferId == 0) && pNeedDepthBuffer) {
      glGenTextures(1, &pTMap->depthBufferId);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pTMap->depthBufferId);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, pTMap->width, pTMap->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pTMap->depthBufferId, 0);
    };

    // enable our draw buffers...
    glDrawBuffers(1, drawBuffers);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      errorlog(status, "Couldn't init framebuffer (errno = %i)", status);
      tmapFreeFrameBuffers(pTMap);
      return false;
    };
  } else {
    // reactivate our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, &pTMap->frameBufferId);
  };

  return true;
};

// free up frame buffers if we no longer need them
void tmapFreeFrameBuffers(texturemap * pTMap) {
  if (pTMap->frameBufferId != 0) {
    glDeleteFramebuffers(1, &pTMap->frameBufferId);
    pTMap->frameBufferId = 0;
  };

  if (pTMap->depthBufferId != 0) {
    glDeleteTextures(1, &pTMap->depthBufferId);
    pTMap->depthBufferId = 0;
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
