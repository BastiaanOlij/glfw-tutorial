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
  GLuint   program;               // our shader program
  GLint    mvpId;                 // our model-view-projection matrix uniform ID
  GLint    textureId;             // our sprite texture sampler ID
  GLuint   texture;               // our sprite texture
  GLint    textureSizeId;         // our texture size uniform ID
  vec2     textureSize;           // size of our sprite texture in pixels
  GLint    spriteSizeId;          // our sprite size uniform ID
  GLint    spriteLeftTopId;       // our sprite left/top uniform ID
  GLuint   spriteCount;           // number of sprites defined in our sprite sheet
  GLuint   maxSpriteCount;        // max number of sprites we can currently hold in memory
  sprite*  sprites;               // array of sprite info
  GLfloat  spriteScale;           // scale to use for sprites
} spritesheet;

typedef void(* SPError)(int, const char*, ...);
void spSetErrorCallback(SPError pCallback);

typedef char*(* SPloadFile)(const char*);
void spSetLoadFileFunc(SPloadFile pLoadFile);

void spLoad(spritesheet* pSP);
void spUnload(spritesheet* pSP);
GLint spAddSprite(spritesheet* pSP, GLfloat pLeft, GLfloat pTop, GLfloat pWidth, GLfloat pHeight);
void spAddSprites(spritesheet* pSP, const sprite* pSprites, int pNumSprites);
void spRender(spritesheet* pSP, const mat4* pProjection, const mat4* pModelView, GLuint pIndex, bool pHorzFlip, bool pVertFlip);

#define newspritesheet(sp) spritesheet sp = { NO_SHADER, -1, -1, 0, -1, { 0.0, 0.0 }, -1, -1, 0, 0, 0, 3.0 }

#ifdef __cplusplus
};
#endif

#ifdef SPRITE_IMPLEMENTATION

//////////////////////////////////////////////////////////
// error handling

SPError spErrCallback = NULL;

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
void spSetErrorCallback(TSError pCallback) {
  spErrCallback = pCallback;
};

//////////////////////////////////////////////////////////
// file handling

SPloadFile spLoadFile = NULL;

// sets the function used to load our file, this way we 
// can change this to something else
void spSetLoadFileFunc(SPloadFile pLoadFile) {
  spLoadFile = pLoadFile;
};

//////////////////////////////////////////////////////////
// sprite sheet

// loads, compiles and links our spritesheet shader
void spLoad(spritesheet* pSP) {
  char* shaderText = NULL;
  GLuint vertexShader = NO_SHADER, fragmentShader = NO_SHADER;

  if ((spErrCallback == NULL) || (spLoadFile == NULL)) {
    return;
  };
  
  shaderText = spLoadFile("spritesheet.vs");
  if (shaderText != NULL) {
    vertexShader = shaderCompile(GL_VERTEX_SHADER, shaderText);
    free(shaderText);
    
    if (vertexShader != NO_SHADER) {
      shaderText = spLoadFile("spritesheet.fs");

      if (shaderText != NULL) {
        fragmentShader = shaderCompile(GL_FRAGMENT_SHADER, shaderText);
        free(shaderText);
        
        if (fragmentShader != NO_SHADER) {
          pSP->program = shaderLink(2, vertexShader, fragmentShader);
          pSP->mvpId = glGetUniformLocation(pSP->program, "mvp");
          if (pSP->mvpId < 0) {
            spErrCallback(pSP->mvpId, "Unknown uniform mvp");
          };
          pSP->textureId = glGetUniformLocation(pSP->program, "spriteTexture");
          if (pSP->textureId < 0) {
            spErrCallback(pSP->textureId, "Unknown uniform spriteTexture");
          };
          pSP->textureSizeId = glGetUniformLocation(pSP->program, "textureSize");
          if (pSP->textureSizeId < 0) {
            spErrCallback(pSP->textureSizeId, "Unknown uniform textureSize");
          };
          pSP->spriteLeftTopId = glGetUniformLocation(pSP->program, "spriteLeftTop");
          if (pSP->spriteLeftTopId < 0) {
            spErrCallback(pSP->spriteLeftTopId, "Unknown uniform spriteLeftTop");
          };
          pSP->spriteSizeId = glGetUniformLocation(pSP->program, "spriteSize");
          if (pSP->spriteSizeId < 0) {
            spErrCallback(pSP->spriteSizeId, "Unknown uniform spriteSize");
          };
        };
                
        // no longer need this...
        glDeleteShader(fragmentShader);
      };
      
      // no longer need this...
      glDeleteShader(vertexShader);
    };
  };
};

// unloads a sprite sheet including freeing up any data
void spUnload(spritesheet* pSP) {
  if (pSP->program != NO_SHADER) {
    glDeleteProgram(pSP->program);
    pSP->program = NO_SHADER;
  };
  
  if (pSP->sprites != 0) {
    free(pSP->sprites);
    pSP->sprites = 0;
    pSP->spriteCount = 0;
    pSP->maxSpriteCount = 0;
  };
};

// add sprite definition to our sprite sheet
GLint spAddSprite(spritesheet* pSP, GLfloat pLeft, GLfloat pTop, GLfloat pWidth, GLfloat pHeight) {
  if (pSP->sprites == 0) {
    pSP->spriteCount = 0;    
    pSP->maxSpriteCount = 10;
    pSP->sprites = (sprite *)malloc(sizeof(sprite) * pSP->maxSpriteCount);
  } else if (pSP->spriteCount>=pSP->maxSpriteCount) {
    pSP->maxSpriteCount += 10;
    pSP->sprites = (sprite *)realloc(pSP->sprites, sizeof(sprite) * pSP->maxSpriteCount);    
  };

  if (pSP->sprites == 0) {
    // couldn't allocate memory
    spErrCallback(-201, "Couldn't allocate memory");
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
  if (pSP->sprites == 0) {
    pSP->spriteCount = 0;    
    pSP->maxSpriteCount = pNumSprites;
    pSP->sprites = (sprite *)malloc(sizeof(sprite) * pSP->maxSpriteCount);
  } else if (pSP->spriteCount+pNumSprites>=pSP->maxSpriteCount) {
    pSP->maxSpriteCount += pNumSprites;
    pSP->sprites = (sprite *)realloc(pSP->sprites, sizeof(sprite) * pSP->maxSpriteCount);    
  };

  if (pSP->sprites == 0) {
    // couldn't allocate memory
    spErrCallback(-201, "Couldn't allocate memory");
    pSP->spriteCount = 0;
    pSP->maxSpriteCount = 0;
  } else {
    memcpy(&(pSP->sprites[pSP->spriteCount]), pSprites, sizeof(sprite) * pNumSprites);
    pSP->spriteCount+=pNumSprites;
  };
};

// renders our tiles using our tile shader
void spRender(spritesheet* pSP, const mat4* pProjection, const mat4* pModelView, GLuint pIndex, bool pHorzFlip, bool pVertFlip) {
  mat4 mvp;
  vec3 tmpvector;
  sprite tmpsprite;
  
  if ((pSP->program != NO_SHADER) && (pIndex < pSP->spriteCount)) {
    glUseProgram(pSP->program);

    // get info about the sprite we're about to draw
    tmpsprite = pSP->sprites[pIndex];

    // set our model-view-projection matrix first
    mat4Copy(&mvp, pProjection);
    mat4Multiply(&mvp, pModelView);

    // and lastly scale our x and y as we use our sprite size and apply our offset
    mat4Scale(&mvp, vec3Set(&tmpvector, pHorzFlip ? -pSP->spriteScale : pSP->spriteScale, pVertFlip ? -pSP->spriteScale : pSP->spriteScale, 1.0));
    mat4Translate(&mvp, vec3Set(&tmpvector, tmpsprite.offsetx, tmpsprite.offsety, 0.0));
    
    if (pSP->mvpId >= 0) {
      glUniformMatrix4fv(pSP->mvpId, 1, false, (const GLfloat *) mvp.m);      
    };
    
    // now tell it which textures to use
    if (pSP->textureId >= 0) {
  		glActiveTexture(GL_TEXTURE0);
  		glBindTexture(GL_TEXTURE_2D, pSP->texture);
  		glUniform1i(pSP->textureId, 0);      
    };
        
    // and tell it what to draw
    if (pSP->textureSizeId >= 0) {
  		glUniform2f(pSP->textureSizeId, pSP->textureSize.x, pSP->textureSize.y);
    };
    if (pSP->spriteLeftTopId >= 0) {
  		glUniform2f(pSP->spriteLeftTopId, tmpsprite.left, tmpsprite.top);      
    };
    if (pSP->spriteSizeId >= 0) {
  		glUniform2f(pSP->spriteSizeId, tmpsprite.width, tmpsprite.height);
    };

    // and draw our triangle
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
  };
};
#endif

#endif
