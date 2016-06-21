/********************************************************
 * gbuffer.h - geo buffer library by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define GBUFF_IMPLEMENTATION
 *
 * Note that OpenGL headers need to be included before 
 * this file is included as it uses several of its 
 * functions. 
 *
 * Revision history:
 * 0.1  23-04-2016  First version with basic functions
 *
 ********************************************************/

#ifndef gbufferh
#define gbufferh

#include "system.h"
#include "math3d.h"
#include "shaders.h"
#include "texturemap.h"
#include "meshnode.h"

#define LIGHTS_MAXSHADOWMAPS 6

// enumeration to record what types of buffers we need
enum GBUFFER_TEXTURE_TYPE {
  GBUFFER_TEXTURE_TYPE_POSITION,  /* Position */
  GBUFFER_TEXTURE_TYPE_NORMAL,    /* Normal */
  GBUFFER_TEXTURE_TYPE_AMBIENT,   /* Ambient */
  GBUFFER_TEXTURE_TYPE_DIFFUSE,   /* Color */
  GBUFFER_TEXTURE_TYPE_SPEC,      /* Specular */
  GBUFFER_NUM_TEXTURES,           /* Number of textures for our gbuffer */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// lights

// define a structure to hold information about our light shader(s)
typedef struct lightShader {
  // standard info
  char              name[50];         // name of the shader
  GLuint            program;          // shader program to use
  GLint             textureUniforms[GBUFFER_NUM_TEXTURES]; // uniforms
  GLint             projectionId;     // ID of our projection matrix
  GLint             lightPosId;       // position of our light
  GLint             lightColId;       // color of our light

  // local lighting (some good info here : http://ogldev.atspace.co.uk/www/tutorial20/tutorial20.html)
  GLint             radiusId;         // radius of influence
  GLint             attConstantId;    // constant attenuation factor
  GLint             attLinearId;      // linear attenuation factor
  GLint             attExpId;         // exponential attenuation factor

  // data for shadowmaps (max LIGHTS_MAXSHADOWMAPS)
  GLint             shadowMapId[LIGHTS_MAXSHADOWMAPS];   // ID of our shadow maps
  GLint             shadowMatId[LIGHTS_MAXSHADOWMAPS];   // ID for our shadow matrices
} lightShader;

// and a structure to hold information about a light
typedef struct lightSource {
  // standard info
  unsigned int      retainCount;      // retain count for this object
  char              name[25];         // name for our light source
  vec3              position;         // position of our light
  vec3              adjPosition;      // position of our light with view matrix applied
  vec3              lightCol;         // color for this light
  float             lightRadius;      // radius of our light source (not applicable to directional light)

  // data for shadowmaps (max LIGHTS_MAXSHADOWMAPS)
  bool              shadowRebuild[LIGHTS_MAXSHADOWMAPS]; // do we need to rebuild our shadow map?
  vec3              shadowLA[LIGHTS_MAXSHADOWMAPS];      // remembering our lookat point for our shadow map
  texturemap *      shadowMap[LIGHTS_MAXSHADOWMAPS];     // shadowmaps for this light
  mat4              shadowMat[LIGHTS_MAXSHADOWMAPS];     // view-projection matrices for this light
} lightSource;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// gBuffer

// define a structure to hold our gbuffer information
typedef struct gBuffer {
  int               width;            // width of this buffer
  int               height;           // height of this buffer
  GLuint            textureIds[GBUFFER_NUM_TEXTURES]; // ids for our textures
  GLuint            depthBufferId;    // ID of our depth texture (if we needed one)  
  GLuint            frameBufferId;    // ID of our framebuffer for render to texture
  GLuint            VAO;              // we need a VAO to render to
  lightShader *     mainPassShader;   // shader to use for our main pass
  lightShader *     pointLightShader; // shader to use for our point lights
} gBuffer;

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// lights

lightShader * newLightShader(const char * pName, const char * pVertexShader, const char * pFragmentShader, llist * pDefines);
void freeLightShader(lightShader * pShader);
bool lightShaderSelect(lightShader * pShader, gBuffer * pBuffer, shaderMatrices * pMatrices, lightSource * pLight);

lightSource * newLightSource(char * pName, const vec3 * pPosition);
void lsRetain(lightSource * pLight);
void lsRelease(lightSource * pLight);
void lsRenderShadowMapForSun(lightSource * pLight, int pMapIdx, int pResolution, float pSize, const vec3 * pEye, meshNode * pScene);
void lsRenderShadowMapsForPointLight(lightSource * pLight, int pResolution, meshNode * pScene);

////////////////////////////////////////////////////////////////////////////////////////////////////////
// gBuffer

gBuffer * newGBuffer(bool pBarrelDist);
void freeGBuffer(gBuffer * pBuffer);
bool gBufferRenderTo(gBuffer * pBuffer, int pWidth, int pHeight);
void gBufferDoMainPass(gBuffer * pBuffer, shaderMatrices * pMatrices, lightSource * pSun);
void gBufferDoPointLight(gBuffer * pBuffer, shaderMatrices * pMatrices, lightSource * pPointLight);

#ifdef __cplusplus
};
#endif

#ifdef GBUFF_IMPLEMENTATION

// precision and color settings for these buffers
GLint  gBuffer_intFormats[GBUFFER_NUM_TEXTURES] = { GL_RGBA32F, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA};
GLenum gBuffer_formats[GBUFFER_NUM_TEXTURES] = { GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA, GL_RGBA };
GLenum gBuffer_types[GBUFFER_NUM_TEXTURES] = { GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE };
GLenum gBuffer_drawBufs[GBUFFER_NUM_TEXTURES] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
char   gBuffer_uniforms[GBUFFER_NUM_TEXTURES][50] = { "worldPos", "normal", "ambient", "diffuse", "specular" };

////////////////////////////////////////////////////////////////////////////////////////////////////////
// lights

// Create a new shader for a lighting stage
lightShader * newLightShader(const char * pName, const char * pVertexShader, const char * pFragmentShader, llist * pDefines) {
  lightShader * newShader = malloc(sizeof(lightShader));
  if (newShader != NULL) {
    GLuint vertexShader = NO_SHADER;
    GLuint fragmentShader = NO_SHADER;

    strcpy(newShader->name, pName);
    newShader->program = NO_SHADER;

    vertexShader = shaderLoad(GL_VERTEX_SHADER, pVertexShader, pDefines);
    fragmentShader = shaderLoad(GL_FRAGMENT_SHADER, pFragmentShader, pDefines);

    if ((vertexShader != NO_SHADER) && (fragmentShader != NO_SHADER)) {
      newShader->program = shaderLink(2, vertexShader, fragmentShader);
      if (newShader->program != NO_SHADER) {
        int i;
        char uName[250];

        for (i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
          newShader->textureUniforms[i] = glGetUniformLocation(newShader->program, gBuffer_uniforms[i]);
          if (newShader->textureUniforms[i] < 0) {
            errorlog(newShader->textureUniforms[i], "Unknown uniform %s:%s", newShader->name, gBuffer_uniforms[i]);  // just log it, may not be a problem
          };
        };

        newShader->projectionId = glGetUniformLocation(newShader->program, "projection");
        if (newShader->projectionId < 0) {
          errorlog(newShader->projectionId, "Unknown uniform %s:projection", newShader->name);
        };

        newShader->lightPosId = glGetUniformLocation(newShader->program, "lightPos");
        if (newShader->lightPosId < 0) {
          errorlog(newShader->lightPosId, "Unknown uniform %s:lightPos", newShader->name);
        };

        newShader->lightColId = glGetUniformLocation(newShader->program, "lightCol");
        if (newShader->lightColId < 0) {
          errorlog(newShader->lightColId, "Unknown uniform %s:lightCol", newShader->name);
        };

        newShader->radiusId = glGetUniformLocation(newShader->program, "radius");
        if (newShader->radiusId < 0) {
          errorlog(newShader->radiusId, "Unknown uniform %s:radius", newShader->name);
        };

        newShader->attConstantId = glGetUniformLocation(newShader->program, "attConstant");
        if (newShader->attConstantId < 0) {
          errorlog(newShader->attConstantId, "Unknown uniform %s:attConstant", newShader->name);
        };

        newShader->attLinearId = glGetUniformLocation(newShader->program, "attLinear");
        if (newShader->attLinearId < 0) {
          errorlog(newShader->attLinearId, "Unknown uniform %s:attLinear", newShader->name);
        };

        newShader->attExpId = glGetUniformLocation(newShader->program, "attExp");
        if (newShader->attExpId < 0) {
          errorlog(newShader->attExpId, "Unknown uniform %s:attExp", newShader->name);
        };

        for (i = 0; i < LIGHTS_MAXSHADOWMAPS; i++) {
          sprintf(uName, "shadowMap[%d]", i);
          newShader->shadowMapId[i] = glGetUniformLocation(newShader->program, uName);
          if (newShader->shadowMapId[i] < 0) {
            errorlog(newShader->shadowMapId[i], "Unknown uniform %s:%s", newShader->name, uName);
          };
          sprintf(uName, "shadowMat[%d]", i);
          newShader->shadowMatId[i] = glGetUniformLocation(newShader->program, uName);
          if (newShader->shadowMatId[i] < 0) {
            errorlog(newShader->shadowMatId[i], "Unknown uniform %s:%s", newShader->name, uName);
          };
        };
      };
    };

    if (vertexShader != NO_SHADER) {
      glDeleteShader(vertexShader);
    };
    if (fragmentShader != NO_SHADER) {
      glDeleteShader(fragmentShader);
    };
  };
  return newShader;
};

// free up memory and objects that make up a light shader
void freeLightShader(lightShader * pShader) {
  if (pShader == NULL) {
    return;
  };

  if (pShader->program != NO_SHADER) {
    glDeleteProgram(pShader->program);
    pShader->program = NO_SHADER;
  };

  free(pShader);
};

float lightMaxDistance(lightSource * pLight) {
  // This calculation is explained here: http://ogldev.atspace.co.uk/www/tutorial36/tutorial36.html
  float maxIllum = fmax(fmax(pLight->lightCol.x, pLight->lightCol.y), pLight->lightCol.z);
  float threshold = 1.0 / 10.0;
  return pLight->lightRadius * sqrtf((maxIllum / threshold) - 0.2);
};

// make a light shader the current shader and load up our uniforms
bool lightShaderSelect(lightShader * pShader, gBuffer * pBuffer, shaderMatrices * pMatrices, lightSource * pLight) {
  int     texture = 0, i;

  if ((pShader == NULL) || (pBuffer == NULL)) {
    return false;
  } else if (pShader->program == NO_SHADER) {
    return false;
  };

  glUseProgram(pShader->program);

  // now tell it which textures to use
  for (i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
    if (pShader->textureUniforms[i] >= 0) {
      glActiveTexture(GL_TEXTURE0 + texture);
      glBindTexture(GL_TEXTURE_2D, pBuffer->textureIds[i]);
      glUniform1i(pShader->textureUniforms[i], texture);
      texture++;      
    };
  };

  if (pShader->projectionId >= 0) {
    glUniformMatrix4fv(pShader->projectionId, 1, false, (const GLfloat *) pMatrices->projection.m);
  };

  // setup the information related to our light source
  if (pShader->lightPosId >= 0) {
    mat4ApplyToVec3(&pLight->adjPosition, &pLight->position, &pMatrices->view);
    glUniform3f(pShader->lightPosId, pLight->adjPosition.x, pLight->adjPosition.y, pLight->adjPosition.z);       
  };

  if (pShader->lightColId >= 0) {
    glUniform3f(pShader->lightColId, pLight->lightCol.x, pLight->lightCol.y, pLight->lightCol.z);
  };

  // setup the information relate to our light strength
  if (pShader->radiusId >= 0) {
    glUniform1f(pShader->radiusId, lightMaxDistance(pLight));
  };
  if (pShader->attConstantId >= 0) {
    glUniform1f(pShader->attConstantId, 0.2);
  };
  if (pShader->attLinearId >= 0) {
    glUniform1f(pShader->attLinearId, 0.4 / (pLight->lightRadius));
  };
  if (pShader->attExpId >= 0) {
    glUniform1f(pShader->attExpId, 0.4 / (pLight->lightRadius * pLight->lightRadius));
  };

  // setup our shadow maps
  for (i = 0; i < LIGHTS_MAXSHADOWMAPS; i++) {
    if (pShader->shadowMapId[i] >= 0) {
      glActiveTexture(GL_TEXTURE0 + texture);
      if (pLight->shadowMap[i] == NULL) {
        glBindTexture(GL_TEXTURE_2D, 0);      
      } else {
        glBindTexture(GL_TEXTURE_2D, pLight->shadowMap[i]->textureId);
      }
      glUniform1i(pShader->shadowMapId[i], texture); 
      texture++;   
    };
    if (pShader->shadowMatId[i] >= 0) {
      mat4 shadowMat;

      // as we only have our position in view space we need to apply the inverse of our view
      mat4Copy(&shadowMat, &(pLight->shadowMat[i]));
      mat4Multiply(&shadowMat, shdMatGetInvView(pMatrices));

      glUniformMatrix4fv(pShader->shadowMatId[i], 1, false, (const GLfloat *) shadowMat.m);
    };
  };  

  return true;
};

lightSource * newLightSource(char * pName, const vec3 * pPosition) {
  lightSource *newLight = malloc(sizeof(lightSource));
  if (newLight != NULL) {
    int i;

    // init default info
    newLight->retainCount = 1;
    strcpy(newLight->name, pName);
    vec3Copy(&newLight->position, pPosition);
    vec3Copy(&newLight->adjPosition, pPosition);
    vec3Set(&newLight->lightCol, 1.0, 1.0, 1.0);

    // init light strenght (ignored for global lights)
    newLight->lightRadius = 100.0;

    // init shadow maps as empty maps
    for (i = 0; i < LIGHTS_MAXSHADOWMAPS; i++) {
      newLight->shadowRebuild[i] = true;
      vec3Set(&newLight->shadowLA[i], 0.0, 0.0, 0.0);
      newLight->shadowMap[i] = NULL;
      mat4Identity(&newLight->shadowMat[i]);
    };
  };
  return newLight;
};

void lsRetain(lightSource * pLight) {
  if (pLight == NULL) {
    errorlog(-1, "Attempted to retain NULL light");
  } else {
    pLight->retainCount++;    
  };
};

void lsRelease(lightSource * pLight) {
  if (pLight == NULL) {
    errorlog(-1, "Attempted to release NULL light");
    return;
  } else if (pLight->retainCount > 1) {
    pLight->retainCount--;
    
    return;
  } else {
    int i;

    for (i = 0; i < LIGHTS_MAXSHADOWMAPS; i++) {
      // lets be nice and cleanup
      if (pLight->shadowMap[i] != NULL) {
        tmapRelease(pLight->shadowMap[i]);
        pLight->shadowMap[i] = NULL;
      };
    };

    free(pLight);
  };
};

// render our shadow map
// note that this likely sets our shadow map FBO and alters our viewport
// calling code needs to reset it back to what it needs
void lsRenderShadowMapForSun(lightSource * pLight, int pMapIdx, int pResolution, float pSize, const vec3 * pEye, meshNode * pScene) {
  vec3 newLookat;

  // prevent rebuilds if we only move a tiny bit....
  newLookat.x = pEye->x - fmod(pEye->x, pSize/100.0);
  newLookat.y = pEye->y - fmod(pEye->y, pSize/100.0);
  newLookat.z = pEye->z - fmod(pEye->z, pSize/100.0);

  if ((pLight->shadowLA[pMapIdx].x != newLookat.x) || (pLight->shadowLA[pMapIdx].y != newLookat.y) || (pLight->shadowLA[pMapIdx].z != newLookat.z)) {
    vec3Copy(&pLight->shadowLA[pMapIdx], &newLookat);
    pLight->shadowRebuild[pMapIdx] = true;
  };

  if (pLight->shadowMap[pMapIdx] == NULL) {
    // create our shadow map if we haven't got one already
    pLight->shadowMap[pMapIdx] = newTextureMap("shadowmap");
  };

  // we'll initialize a shadow map for our sun
  if (pLight->shadowRebuild[pMapIdx] == false) {
    // reuse it as is...
  } else if (pScene == NULL) {
    // nothing to render..
  } else if (tmapRenderToShadowMap(pLight->shadowMap[pMapIdx], pResolution, pResolution)) {
    mat4            tmpmatrix;
    vec3            tmpvector;
    shaderMatrices  matrices;

    // rest our last used material
    matResetLastUsed();

    // set our viewport
    glViewport(0, 0, pResolution, pResolution);

    // enable and configure our backface culling, note that here we cull our front facing polygons
    // to minimize shading artifacts
    glEnable(GL_CULL_FACE);   // enable culling
    glFrontFace(GL_CW);       // clockwise
    glCullFace(GL_FRONT);     // frontface culling

    // enable our depth test
    glEnable(GL_DEPTH_TEST);  // check our depth
    glDepthMask(GL_TRUE);     // enable writing to our depth buffer

    // disable alpha blending  
    glDisable(GL_BLEND);

    // solid polygons
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    

    // clear our depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);      

    // need to create our projection matrix first
    // for our sun we need an orthographic projection as rays of sunlight pretty much are parallel to each other.
    // if this was a spotlight a perspective projection gives the best result
    mat4Identity(&tmpmatrix);
    mat4Ortho(&tmpmatrix, -pSize, pSize, -pSize, pSize, -50000.0, 50000.0);
    shdMatSetProjection(&matrices, &tmpmatrix);

    // We are going to adjust our sun's position based on our camera position.
    // We position the sun such that our camera location would be at Z = 0.
    // Our near plane is actually behind our 'sun' which gives us some wiggleroom.
    // Note that this should result in the same calculations for all 3 shadow maps at this time
    vec3Copy(&pLight->adjPosition, &pLight->position);
    vec3Normalise(&pLight->adjPosition);  // normalize our sun position vector
    vec3Mult(&pLight->adjPosition, 10000.0); // move the sun far enough away
    vec3Add(&pLight->adjPosition, &pLight->shadowLA[pMapIdx]); // position in relation to our camera

    // Now we can create our view matrix, here we use a lookat matrix from our sun looking towards our camera position.
    // There is an argument to use our lookat point instead as in worst case scenarios half our of shadowmap could
    // relate to what is behind our camera but using our lookat point risks not covering enough with our shadowmap.
    //
    // Note that for our 'up-vector' we're using an Z-axis aligned vector. This is because our sun will be straight
    // up at noon and we'd get an unusable view matrix. An Z-axis aligned vector assumes that our sun goes from east
    // to west along the X/Y axis and the Z of our sun will be 0. Our 'up-vector' thus points due north (or south
    // depending on your definition).
    // If you do not align your coordinate system to a compass you'll have to calculate an up-vector that points to your
    // north or south 
    mat4Identity(&tmpmatrix);
    mat4LookAt(&tmpmatrix, &pLight->adjPosition, &pLight->shadowLA[pMapIdx], vec3Set(&tmpvector, 0.0, 0.0, 1.0));
    shdMatSetView(&matrices, &tmpmatrix);

    // now we override our eye position to be at our camera position, this is important for our LOD calculations
    shdMatSetEyePos(&matrices, &pLight->shadowLA[pMapIdx]);

    // and now render our scene for shadow maps (note that we only render materials that have a shadow shader and we ignore transparent objects)
    meshNodeShadowMap(pScene, &matrices);    

    // now remember our view-projection matrix, we need it later on when rendering our scene
    mat4Copy(&pLight->shadowMat[pMapIdx], shdMatGetViewProjection(&matrices));

    // we can keep it.
    pLight->shadowRebuild[pMapIdx] = false;

    // and we're done
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };
};

void lsRenderShadowMapsForPointLight(lightSource * pLight, int pResolution, meshNode * pScene) {
  int i;

  vec3 lookats[] = {
       0.0, -100.0,    0.0, 
     100.0,    0.0,    0.0, 
    -100.0,    0.0,    0.0, 
       0.0,  100.0,    0.0, 
       0.0,    0.0,  100.0, 
       0.0,    0.0, -100.0, 
  };

  // as we're using our light position and its the same for all shadow maps we only check our flag on the first
  if ((pLight->shadowLA[0].x != pLight->position.x) || (pLight->shadowLA[0].y != pLight->position.y) || (pLight->shadowLA[0].z != pLight->position.z)) {
    vec3Copy(&pLight->shadowLA[0], &pLight->position);
    pLight->shadowRebuild[0] = true;
  };

  // we'll initialize our shadow maps for our point light
  if (pLight->shadowRebuild[0] == false) {
    // reuse it as is...
  } else if (pScene == NULL) {
    // nothing to render..
  } else {
    for (i = 0; i < 6; i++) {
      if (pLight->shadowMap[i] == NULL) {
        // create our shadow map if we haven't got one already
        pLight->shadowMap[i] = newTextureMap("shadowmap");
      };

      if (tmapRenderToShadowMap(pLight->shadowMap[i], pResolution, pResolution)) {
        mat4            tmpmatrix;
        vec3            tmpvector, lookat;
        shaderMatrices  matrices;

        // rest our last used material
        matResetLastUsed();

        // set our viewport
        glViewport(0, 0, pResolution, pResolution);

        // enable and configure our backface culling, note that here we cull our front facing polygons
        // to minimize shading artifacts
        glEnable(GL_CULL_FACE);   // enable culling
        glFrontFace(GL_CW);       // clockwise
        glCullFace(GL_FRONT);     // frontface culling

        // enable our depth test
        glEnable(GL_DEPTH_TEST);  // check our depth
        glDepthMask(GL_TRUE);     // enable writing to our depth buffer

        // disable alpha blending  
        glDisable(GL_BLEND);

        // solid polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    

        // clear our depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);      

        // set our projection
        mat4Identity(&tmpmatrix);
        mat4Projection(&tmpmatrix, 90.0, 1.0, 1.0, lightMaxDistance(pLight) * 1.5);
        shdMatSetProjection(&matrices, &tmpmatrix); // call our set function to reset our flags

        // now make a view based on our light position
        mat4Identity(&tmpmatrix);
        vec3Copy(&lookat, &pLight->position);
        vec3Add(&lookat, &lookats[i]);
        mat4LookAt(&tmpmatrix, &pLight->position, &lookat, vec3Set(&tmpvector, 0.0, 1.0, 0.0));
        shdMatSetView(&matrices, &tmpmatrix);

        // and render
        meshNodeShadowMap(pScene, &matrices);

        // now remember our view-projection matrix, we need it later on when rendering our scene
        mat4Copy(&pLight->shadowMat[i], shdMatGetViewProjection(&matrices));

        // we can keep it.
        pLight->shadowRebuild[i] = false;

        // and we're done
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      };
    };
  };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// gBuffer

// create a new geometry buffer
gBuffer * newGBuffer(bool pBarrelDist) {
  gBuffer * newBuffer = malloc(sizeof(gBuffer));
  if (newBuffer != NULL) {
    llist * defines = newVarcharList();

    // we create our texture object but do not initialize anything until a size is known
    newBuffer->width = 0;
    newBuffer->height = 0;
    glGenTextures(GBUFFER_NUM_TEXTURES, newBuffer->textureIds);
    glGenTextures(1, &newBuffer->depthBufferId);

    newBuffer->frameBufferId = 0;
    newBuffer->VAO = GL_UNDEF_OBJ;

    if (pBarrelDist) {
      vclistAddString(defines, "barreldist");
    };

    newBuffer->mainPassShader = newLightShader("geomainpass", "geomainpass.vs", "geomainpass.fs", defines);
    newBuffer->pointLightShader = newLightShader("geopointlight", "geopointlight.vs", "geopointlight.fs", defines);

    // no longer need our defines
    if (defines != NULL) {
      llistFree(defines);
    };
  };
  return newBuffer;
};

// free up all resources related to a geometry buffer
void freeGBuffer(gBuffer * pBuffer) {
  if (pBuffer != NULL) {
    return;
  };

  if (pBuffer->pointLightShader != NULL) {
    freeLightShader(pBuffer->pointLightShader);
    pBuffer->pointLightShader = NULL;
  };

  if (pBuffer->mainPassShader != NULL) {
    freeLightShader(pBuffer->mainPassShader);
    pBuffer->mainPassShader = NULL;
  };

  if (pBuffer->VAO != GL_UNDEF_OBJ) {
    glDeleteVertexArrays(1, &(pBuffer->VAO));    
    pBuffer->VAO = GL_UNDEF_OBJ;
  };

  if (pBuffer->frameBufferId != 0) {
    glDeleteFramebuffers(1, &pBuffer->frameBufferId);
    pBuffer->frameBufferId = 0;
  };

  glDeleteTextures(GBUFFER_NUM_TEXTURES, pBuffer->textureIds);
  glDeleteTextures(1, &pBuffer->depthBufferId);

  free(pBuffer);
};

// make a geometry buffer the rendering target resizing/initializing the buffer if required to match the given size
bool gBufferRenderTo(gBuffer * pBuffer, int pWidth, int pHeight) {
  if (pBuffer == NULL) {
    return false;
  };

  if (((pBuffer->width != pWidth) || (pBuffer->height != pHeight)) && (pBuffer->frameBufferId != 0)) {
    glDeleteFramebuffers(1, &pBuffer->frameBufferId);
    pBuffer->frameBufferId = 0;
  };

  if (pBuffer->frameBufferId == 0) {
    // need to recreate it!
    GLenum status;
    int i;

    // remember these
    pBuffer->width = pWidth;
    pBuffer->height = pHeight;

    // create our frame buffer
    glGenFramebuffers(1, &pBuffer->frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, pBuffer->frameBufferId);

    // init our texture buffers
    for (i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
      glBindTexture(GL_TEXTURE_2D, pBuffer->textureIds[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, gBuffer_intFormats[i], pBuffer->width, pBuffer->height, 0, gBuffer_formats[i], gBuffer_types[i], NULL); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, pBuffer->textureIds[i], 0);
    };

    // create our depth buffer 
    glBindTexture(GL_TEXTURE_2D, pBuffer->depthBufferId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, pBuffer->width, pBuffer->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pBuffer->depthBufferId, 0);

    // and finalize our frame buffer
    glDrawBuffers(GBUFFER_NUM_TEXTURES, gBuffer_drawBufs);
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      errorlog(status, "Couldn't init framebuffer (errno = %i)", status);
      glDeleteFramebuffers(1, &pBuffer->frameBufferId);
      pBuffer->frameBufferId = 0;
      return false;
    } else {
      errorlog(0, "Created gbuffer %i, %i", pBuffer->width, pBuffer->height);
    };
  } else {
    // we can reuse it!
    glBindFramebuffer(GL_FRAMEBUFFER, pBuffer->frameBufferId);
  };

  // set our viewport
  glViewport(0, 0, pBuffer->width, pBuffer->height);

  return true;
};

// process our main lighting to create our initial render result
void gBufferDoMainPass(gBuffer * pBuffer, shaderMatrices * pMatrices, lightSource * pSun) {
  if (pBuffer == NULL) {
    return;
  } else if (pBuffer->mainPassShader == NULL) {
    return;
  } else if (pBuffer->mainPassShader->program == NO_SHADER) {
    return;
  };

  // setup our rendering
  glDisable(GL_DEPTH_TEST); // no need to check our depth buffer
  glDepthMask(GL_FALSE);    // no need to write to our depth buffer
  glDisable(GL_CULL_FACE);  // no need to cull any backfaces
  glDisable(GL_BLEND);      // no blending for our main pass!

  // select our program
  lightShaderSelect(pBuffer->mainPassShader, pBuffer, pMatrices, pSun);
  
  // select our VAO
  if (pBuffer->VAO == GL_UNDEF_OBJ) {
    glGenVertexArrays(1, &(pBuffer->VAO));
  };
  glBindVertexArray(pBuffer->VAO);

  // and draw
  glDrawArrays(GL_TRIANGLES, 0, 3 * 2);

  // and clear our selected vertex array object
  glBindVertexArray(0);
};

// draw a point light, note that we are assuming gBufferDoMainPass was called before we call this and
// that we have correctly setup blending
//
// First we do our global lighting
// gBufferDoMainPass(geoBuffer, &matrices, sun);  
//
// now use blending for our additional lights
// glEnable(GL_BLEND);
// glBlendEquation(GL_FUNC_ADD);
// glBlendFunc(GL_ONE, GL_ONE);
//
// loop through our point lights
// for(i = 0; i < NUM_LIGHTS; i++) {
//   gBufferDoPointLight(geoBuffer, &matrices, pointLights[i]); 
//}
void gBufferDoPointLight(gBuffer * pBuffer, shaderMatrices * pMatrices, lightSource * pPointLight) {
  if (pBuffer == NULL) {
    return;
  } else if (pBuffer->pointLightShader == NULL) {
    return;
  } else if (pBuffer->pointLightShader->program == NO_SHADER) {
    return;
  };

  // select our program
  lightShaderSelect(pBuffer->pointLightShader, pBuffer, pMatrices, pPointLight);
  
  if (pPointLight->adjPosition.z < pPointLight->lightRadius) {
    // select our VAO
    if (pBuffer->VAO == GL_UNDEF_OBJ) {
      glGenVertexArrays(1, &(pBuffer->VAO));
    };
    glBindVertexArray(pBuffer->VAO);

    // and draw
    glDrawArrays(GL_TRIANGLE_FAN, 0, 1 + 37);

    // and clear our selected vertex array object
    glBindVertexArray(0);
  };
};

#endif /* GBUFF_IMPLEMENTATION */

#endif /* !gbufferh */