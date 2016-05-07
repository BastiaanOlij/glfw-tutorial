/********************************************************
 * Tilemap contains logic to render sprites from a 
 * spritesheet
 *
 * This implementation loads a single texture as a
 * spritesheet but allows for drawing non-uniform sprites
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define SPRITE_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. It also requires math3d.h and shaders.h
 * to be included before hand
 *
 ********************************************************/

#ifndef spritesheeth
#define spritesheeth

// standard libraries we need...
#include <stdarg.h>
#include "system.h"
#include "math3d.h"
#include "shaders.h"
#include "texturemap.h"

#ifdef __cplusplus
extern "C" {
#endif

// structure to keep data about our spritesheet
typedef struct sprite {
  GLfloat left;                   // left position of our sprite in our texture
  GLfloat top;                    // top position of our sprite in our texture
  GLfloat width;                  // width of our sprite
  GLfloat height;                 // height of our sprite
  GLfloat offsetx;                // horizontal offset
  GLfloat offsety;                // vertical offset
} sprite;

typedef struct spritesheet {
  GLuint        vao;
  GLuint        program;          // our shader program
  GLint         mvpId;            // our model-view-projection matrix uniform ID
  GLint         textureId;        // our sprite texture sampler ID
  GLint         textureSizeId;    // our texture size uniform ID
  texturemap *  texture;          // our sprite texture
  GLint         spriteSizeId;     // our sprite size uniform ID
  GLint         spriteLeftTopId;  // our sprite left/top uniform ID
  GLuint        spriteCount;      // number of sprites defined in our sprite sheet
  GLuint        maxSpriteCount;   // max number of sprites we can currently hold in memory
  sprite*       sprites;          // array of sprite info
  GLfloat       spriteScale;      // scale to use for sprites
} spritesheet;

spritesheet * newSpriteSheet(void);
void spFree(spritesheet * pSP);
void spSetTexture(spritesheet * pSP, texturemap * pTexture);
GLint spAddSprite(spritesheet* pSP, GLfloat pLeft, GLfloat pTop, GLfloat pWidth, GLfloat pHeight);
void spAddSprites(spritesheet* pSP, const sprite* pSprites, int pNumSprites);
void spRender(spritesheet* pSP, shaderMatrices * pMatrices, GLuint pIndex, bool pHorzFlip, bool pVertFlip);

#ifdef __cplusplus
};
#endif

#ifdef SPRITE_IMPLEMENTATION

//////////////////////////////////////////////////////////
// sprite sheet

// loads, compiles and links our spritesheet shader
void spLoadShader(spritesheet* pSP) {
  GLuint vertexShader = NO_SHADER, fragmentShader = NO_SHADER;

  vertexShader = shaderLoad(GL_VERTEX_SHADER, "spritesheet.vs", NULL);
  fragmentShader = shaderLoad(GL_FRAGMENT_SHADER, "spritesheet.fs", NULL);

  if ((vertexShader != NO_SHADER) && (fragmentShader != NO_SHADER)) {
    pSP->program = shaderLink(2, vertexShader, fragmentShader);
    if (pSP->program == NO_SHADER) {
      errorlog(-1, "Unable to init sprite shader");
    } else {
      pSP->mvpId = glGetUniformLocation(pSP->program, "mvp");
      if (pSP->mvpId < 0) {
        errorlog(pSP->mvpId, "Unknown uniform mvp");
      };
      pSP->textureId = glGetUniformLocation(pSP->program, "spriteTexture");
      if (pSP->textureId < 0) {
        errorlog(pSP->textureId, "Unknown uniform spriteTexture");
      };
      pSP->textureSizeId = glGetUniformLocation(pSP->program, "textureSize");
      if (pSP->textureSizeId < 0) {
        errorlog(pSP->textureSizeId, "Unknown uniform textureSize");
      };
      pSP->spriteLeftTopId = glGetUniformLocation(pSP->program, "spriteLeftTop");
      if (pSP->spriteLeftTopId < 0) {
        errorlog(pSP->spriteLeftTopId, "Unknown uniform spriteLeftTop");
      };
      pSP->spriteSizeId = glGetUniformLocation(pSP->program, "spriteSize");
      if (pSP->spriteSizeId < 0) {
        errorlog(pSP->spriteSizeId, "Unknown uniform spriteSize");
      };
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

// init, initialises base values for our spritesheet, kinda like our constructor
spritesheet * newSpriteSheet(void) {
  spritesheet * newsp = (spritesheet *) malloc(sizeof(spritesheet));
  if (newsp != NULL) {
    glGenVertexArrays(1, &newsp->vao);
    newsp->program          = NO_SHADER;
    newsp->mvpId            = -1;
    newsp->textureId        = -1;
    newsp->textureSizeId    = -1;
    newsp->texture          = NULL;
    newsp->spriteSizeId     = -1;
    newsp->spriteLeftTopId  = -1;
    newsp->spriteCount      = 0;
    newsp->maxSpriteCount   = 0;
    newsp->sprites          = NULL;
    newsp->spriteScale      = 1.0;

    spLoadShader(newsp);
  };
  return newsp;
};

void spFree(spritesheet * pSP) {
  if (pSP == NULL) {
    return;
  };

  if (pSP->program != NO_SHADER) {
    glDeleteProgram(pSP->program);
    pSP->program = NO_SHADER;
  };

  tmapRelease(pSP->texture);
  
  if (pSP->sprites != 0) {
    free(pSP->sprites);
    pSP->sprites = 0;
    pSP->spriteCount = 0;
    pSP->maxSpriteCount = 0;
  };

  glDeleteVertexArrays(1, &pSP->vao);
  free(pSP);
};

void spSetTexture(spritesheet * pSP, texturemap * pTexture) {
  if (pSP == NULL) {
    return;
  } else if (pSP->texture == pTexture) {
    return;
  } else {
    if (pSP->texture != NULL) {
      tmapRelease(pSP->texture);
    };
    pSP->texture = pTexture;
    if (pSP->texture != NULL) {
      tmapRetain(pSP->texture);
    };    
  };
};

// add sprite definition to our sprite sheet
GLint spAddSprite(spritesheet* pSP, GLfloat pLeft, GLfloat pTop, GLfloat pWidth, GLfloat pHeight) {
  if (pSP->sprites == NULL) {
    pSP->spriteCount = 0;    
    pSP->maxSpriteCount = 10;
    pSP->sprites = (sprite *)malloc(sizeof(sprite) * pSP->maxSpriteCount);
  } else if (pSP->spriteCount>=pSP->maxSpriteCount) {
    pSP->maxSpriteCount += 10;
    pSP->sprites = (sprite *)realloc(pSP->sprites, sizeof(sprite) * pSP->maxSpriteCount);    
  };

  if (pSP->sprites == 0) {
    // couldn't allocate memory
    errorlog(-201, "Couldn't allocate memory");
    pSP->spriteCount = 0;
    pSP->maxSpriteCount = 0;
    
    return -1;
  } else {
    pSP->sprites[pSP->spriteCount].left = pLeft;
    pSP->sprites[pSP->spriteCount].top = pTop;
    pSP->sprites[pSP->spriteCount].width = pWidth;
    pSP->sprites[pSP->spriteCount].height = pHeight;
    pSP->spriteCount++;
    return pSP->spriteCount - 1;
  };
};

// adds an array of sprites
void spAddSprites(spritesheet* pSP, const sprite* pSprites, int pNumSprites) {
  if (pSP->sprites == NULL) {
    pSP->spriteCount = 0;    
    pSP->maxSpriteCount = pNumSprites;
    pSP->sprites = (sprite *)malloc(sizeof(sprite) * pSP->maxSpriteCount);
  } else if (pSP->spriteCount+pNumSprites>=pSP->maxSpriteCount) {
    pSP->maxSpriteCount += pNumSprites;
    pSP->sprites = (sprite *)realloc(pSP->sprites, sizeof(sprite) * pSP->maxSpriteCount);    
  };

  if (pSP->sprites == 0) {
    // couldn't allocate memory
    errorlog(-201, "Couldn't allocate memory");
    pSP->spriteCount = 0;
    pSP->maxSpriteCount = 0;
  } else {
    memcpy(&(pSP->sprites[pSP->spriteCount]), pSprites, sizeof(sprite) * pNumSprites);
    pSP->spriteCount+=pNumSprites;
  };
};

// renders our tiles using our tile shader
void spRender(spritesheet* pSP, shaderMatrices * pMatrices, GLuint pIndex, bool pHorzFlip, bool pVertFlip) {
  mat4 model;
  vec3 tmpvector;
  sprite tmpsprite;
  
  if ((pSP->program != NO_SHADER) && (pIndex < pSP->spriteCount)) {
    glUseProgram(pSP->program);

    // get info about the sprite we're about to draw
    tmpsprite = pSP->sprites[pIndex];

    // set our model matrix to scale our x and y as we use our sprite size and apply our offset
    mat4Copy(&model, &pMatrices->model);
    mat4Scale(&model, vec3Set(&tmpvector, pHorzFlip ? -pSP->spriteScale : pSP->spriteScale, pVertFlip ? -pSP->spriteScale : pSP->spriteScale, 1.0));
    mat4Translate(&model, vec3Set(&tmpvector, tmpsprite.offsetx, tmpsprite.offsety, 0.0));
    shdMatSetModel(pMatrices, &model);
    
    if (pSP->mvpId >= 0) {
      glUniformMatrix4fv(pSP->mvpId, 1, false, (const GLfloat *) shdMatGetMvp(pMatrices)->m);      
    };
    
    // now tell it which textures to use
    if (pSP->textureId >= 0) {
  		glActiveTexture(GL_TEXTURE0);
  		glBindTexture(GL_TEXTURE_2D, pSP->texture->textureId);
  		glUniform1i(pSP->textureId, 0);      
    };
        
    // and tell it what to draw
    if (pSP->textureSizeId >= 0) {
  		glUniform2f(pSP->textureSizeId, pSP->texture->width, pSP->texture->height);
    };
    if (pSP->spriteLeftTopId >= 0) {
  		glUniform2f(pSP->spriteLeftTopId, tmpsprite.left, tmpsprite.top);      
    };
    if (pSP->spriteSizeId >= 0) {
  		glUniform2f(pSP->spriteSizeId, tmpsprite.width, tmpsprite.height);
    };

    // and draw our triangle
    glBindVertexArray(pSP->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
  };
};
#endif

#endif
