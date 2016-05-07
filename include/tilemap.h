/********************************************************
 * Tilemap contains logic to render tiles with
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define TILEMAP_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. It also requires math3d.h and shaders.h
 * to be included before hand
 *
 ********************************************************/

#ifndef tilemaph
#define tilemaph

// standard libraries we need...
#include <stdarg.h>
#include "system.h"
#include "math3d.h"
#include "shaders.h"
#include "texturemap.h"

#ifdef __cplusplus
extern "C" {
#endif

// structure for containing our tile shader
typedef struct tileshader {
  GLuint          vao;
  GLuint          program;
  GLint           mvpId;
  GLint           mapdataId;
  GLint           mapSizeId;
  texturemap *    mapData;
  GLfloat         mapScale;
  GLint           tileId;
  GLint           tilesPerSideId;
  GLint           textureSizeId;
  GLuint          tilesPerSide;
  texturemap *    texture;
} tileshader;

tileshader * newTileShader(void);
void tsFree(tileshader * pTS);
void tsSetMapData(tileshader * pTS, texturemap * pMapData);
void tsSetTexture(tileshader * pTS, texturemap * pTexture);
void tsRender(tileshader* pTS, shaderMatrices * pMatrices);

#ifdef __cplusplus
};
#endif

#ifdef TILEMAP_IMPLEMENTATION

//////////////////////////////////////////////////////////
// tile shaders

// loads, compiles and links our tileshader
void tsLoadShader(tileshader* pTS) {
  GLuint vertexShader = NO_SHADER, fragmentShader = NO_SHADER;

  errorlog(0, "Loading tile shader");

  vertexShader = shaderLoad(GL_VERTEX_SHADER, "tilemap.vs", NULL);
  fragmentShader = shaderLoad(GL_FRAGMENT_SHADER, "tilemap.fs", NULL);

  if ((vertexShader != NO_SHADER) && (fragmentShader != NO_SHADER)) {
    pTS->program = shaderLink(2, vertexShader, fragmentShader);
    if (pTS->program == NO_SHADER) {
      errorlog(-1, "Unable to init tile shader");
    } else {
      pTS->mvpId = glGetUniformLocation(pTS->program, "mvp");
      if (pTS->mvpId < 0) {
        errorlog(pTS->mvpId, "Unknown uniform mvp");
      };
      pTS->mapdataId = glGetUniformLocation(pTS->program, "mapdata");
      if (pTS->mapdataId < 0) {
        errorlog(pTS->mapdataId, "Unknown uniform mapdata");
      };
      pTS->mapSizeId = glGetUniformLocation(pTS->program, "mapSize");
      if (pTS->mapSizeId < 0) {
        errorlog(pTS->mapSizeId, "Unknown uniform mapSize");
      };
      pTS->tileId = glGetUniformLocation(pTS->program, "tiles");
      if (pTS->tileId < 0) {
        errorlog(pTS->tileId, "Unknown uniform tiles");
      };
      pTS->tilesPerSideId = glGetUniformLocation(pTS->program, "tilesPerSide");
      if (pTS->tilesPerSideId < 0) {
        errorlog(pTS->tilesPerSideId, "Unknown uniform tilesPerSide");
      };
      pTS->textureSizeId = glGetUniformLocation(pTS->program, "textureSize");
      if (pTS->textureSizeId < 0) {
        errorlog(pTS->textureSizeId, "Unknown uniform textureSize");
      };

      errorlog(0, "Loaded tile shader");
    };
  };

  if (fragmentShader != NO_SHADER) {
    // no longer need this...
    glDeleteShader(fragmentShader);
  };

  if (vertexShader != NO_SHADER) {
    // no longer need this...
    glDeleteShader(vertexShader);
  };
};

//////////////////////////////////////////////////////////
// initialisation

// create a new tileshader
tileshader * newTileShader(void) {
  tileshader * newts = (tileshader *) malloc(sizeof(tileshader));
  if (newts != NULL) {
    glGenVertexArrays(1, &newts->vao);
    newts->program          = NO_SHADER;
    newts->mvpId            = -1;
    newts->mapdataId        = -1;
    newts->mapSizeId        = -1;
    newts->mapData          = NULL;
    newts->mapScale         = 100.0;
    newts->tileId           = -1;
    newts->texture          = NULL;
    newts->tilesPerSideId   = -1;
    newts->tilesPerSide     = 8;
    newts->textureSizeId    = -1;

    tsLoadShader(newts);
  };
  return newts;
};

void tsFree(tileshader * pTS) {
  if (pTS == NULL) {
    return;
  };

  tsSetMapData(pTS, NULL);
  tsSetTexture(pTS, NULL);

  if (pTS->program != NO_SHADER) {
    glDeleteProgram(pTS->program);
    pTS->program = NO_SHADER;
  };

  glDeleteVertexArrays(1, &pTS->vao);

  free(pTS);
};

// set the texturemap containing our map data
void tsSetMapData(tileshader * pTS, texturemap * pMapData) {
  if (pTS == NULL) {
    return;
  } else if (pTS->mapData == pMapData) {
    return;
  } else {
    if (pTS->mapData != NULL) {
      tmapRelease(pTS->mapData);
    };
    pTS->mapData = pMapData;
    if (pTS->mapData != NULL) {
      tmapRetain(pTS->mapData);
    };    
  };
};

// set the texturemap for our tiles
void tsSetTexture(tileshader * pTS, texturemap * pTexture) {
  if (pTS == NULL) {
    return;
  } else if (pTS->texture == pTexture) {
    return;
  } else {
    if (pTS->texture != NULL) {
      tmapRelease(pTS->texture);
    };
    pTS->texture = pTexture;
    if (pTS->texture != NULL) {
      tmapRetain(pTS->texture);
    };    
  };
};

// renders our tiles using our tile shader
void tsRender(tileshader* pTS, shaderMatrices * pMatrices) {
  mat4 model, mvp;
  vec3 tmpvector;
  
  if (pTS->program != NO_SHADER) {
    glUseProgram(pTS->program);

    // Scale our x and y as they are unified
    mat4Copy(&model, &pMatrices->model);
    mat4Scale(&model, vec3Set(&tmpvector, pTS->mapScale, pTS->mapScale, 1.0));
    shdMatSetModel(pMatrices, &model);

    // set our model-view-projection matrix first
//    mat4Copy(&mvp, &pMatrices->projection);
//    mat4Multiply(&mvp, &pMatrices->view);
    
    // and lastly scale our x and y as they are unified
//    mat4Scale(&mvp, vec3Set(&tmpvector, pTS->mapScale, pTS->mapScale, 1.0));

    if (pTS->mvpId >= 0) {
      // glUniformMatrix4fv(pTS->mvpId, 1, false, (const GLfloat *) mvp.m);      
      glUniformMatrix4fv(pTS->mvpId, 1, false, (const GLfloat *) shdMatGetMvp(pMatrices)->m);      
    };
    
    // now tell it which textures to use and some more info about it
    if ((pTS->mapData != NULL) && (pTS->texture != NULL)) {
      if (pTS->mapdataId >= 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pTS->mapData->textureId);
        glUniform1i(pTS->mapdataId, 0);      
      };

      if (pTS->mapSizeId >= 0) {
        glUniform2f(pTS->mapSizeId, pTS->mapData->width, pTS->mapData->height);      
      };

      if (pTS->tileId >= 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pTS->texture->textureId);
        glUniform1i(pTS->tileId, 1);      
      };
      if (pTS->tilesPerSideId >= 0) {
        glUniform1i(pTS->tilesPerSideId, pTS->tilesPerSide);      
      };
      if (pTS->textureSizeId >= 0) {
        glUniform1f(pTS->textureSizeId, pTS->texture->width); // we assume square      
      };

    
      // and draw our triangles
      glBindVertexArray(pTS->vao);
      glDrawArrays(GL_TRIANGLES, 0, pTS->mapData->width * pTS->mapData->height * 3 * 2);
    };
  };
};

#endif

#endif
