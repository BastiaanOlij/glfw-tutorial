/********************************************************
 * Engine is our container for our main program logic
 * 
 * This allows us to only place code in main.c that
 * interacts with the user through our framework and
 * thus change framework if needed.
 * 
 ********************************************************/

#include "engine.h"

//////////////////////////////////////////////////////////
// For now we just some global state to make life easy

EngineKeyPressed engineKeyPressedCallback = NULL;

// info about what is supported, this should move into our shader library at some point
int           maxPatches = 0; // how many patches can our shader output
int           maxTessLevel = 0; // what is the maximum tesselation level we support

// object info
llist *       materials = NULL;
texturemap *  heightMap = NULL;
meshNode *    scene = NULL;
meshNode *    tieNodes[10] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// and some globals for our fonts
FONScontext * fs = NULL;
int           font = FONS_INVALID;
float         lineHeight = 0.0f;

// shaders
shaderInfo *  skyboxShader = NULL;
shaderInfo *  hmapShader = NULL;
shaderInfo *  colorShader = NULL;
shaderInfo *  texturedShader = NULL;
shaderInfo *  reflectShader = NULL;
shaderInfo *  billboardShader = NULL;
shaderInfo *  solidShadow = NULL;
shaderInfo *  textureShadow = NULL;

// lights
lightSource   sun;

// our camera
mat4          view;
vec3          camera_eye = { 0.0, 1000.0, 1300.0 };
vec3          camera_lookat =  { 0.0, 1000.0, 0.0 };

// and some runtime variables.
bool          wireframe = false;
bool          showinfo = true;
bool          bounds = false;
double        frames = 0.0f;
double        fps = 0.0f;
double        lastframes = 0.0f;
double        lastsecs = 0.0f;

//////////////////////////////////////////////////////////
// keyboard handling

// sets a callback that allows us to test if a key is
// pressed
void engineSetKeyPressedCallback(EngineKeyPressed pCallback) {
  engineKeyPressedCallback = pCallback;
};

//////////////////////////////////////////////////////////
// fonts
void load_font() {
  // we start with creating a font context that tells us about the font we'll be rendering
  fs = gl3fonsCreate(512, 512, FONS_ZERO_TOPLEFT);
  if (fs != NULL) {
    // then we load our font
    #ifdef __APPLE__
      font = fonsAddFont(fs, "sans", "Fonts/DroidSerif-Regular.ttf");
    #else
      font = fonsAddFont(fs, "sans", "Resoures\\Fonts\\DroidSerif-Regular.ttf");
    #endif
    if (font != FONS_INVALID) {
      // setup our font
      fonsSetColor(fs, gl3fonsRGBA(255,255,255,255)); // white
      fonsSetSize(fs, 16.0f); // 16 point font
      fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP); // left/top aligned
      fonsVertMetrics(fs, NULL, NULL, &lineHeight);
    } else {
      errorlog(-201, "Couldn't load DroidSerif-Regular.ttf");       
    };
  } else {
    errorlog(-200, "Couldn't create our font context");
  };
};

void unload_font() {
  if (fs != NULL) {
    gl3fonsDelete(fs);
    fs = NULL;
  };
};

//////////////////////////////////////////////////////////
// Shaders

void load_shaders() {
  // init some paths
  #ifdef __APPLE__
    shaderSetPath("Shaders/");
  #else
    shaderSetPath("Resources\\Shaders\\");
  #endif

  // get info about our tesselation capabilities
  glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatches);
  errorlog(0, "Supported patches: %d", maxPatches);
  if (maxPatches >= 4) {
    // setup using quads
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    errorlog(0, "Maximum supported tesselation level: %d", maxTessLevel);
  };

  skyboxShader = newShader("skybox", "skybox.vs", NULL, NULL, NULL, "skybox.fs");

  if (maxPatches >= 4) {
    hmapShader = newShader("hmap", "hmap_ts.vs", "hmap_ts.ts", "hmap_ts.te", "hmap_ts.gs", "hmap_ts.fs");
  } else {
    hmapShader = newShader("hmap", "hmap.vs", NULL, NULL, NULL, "hmap.fs");
  };

  colorShader = newShader("flatcolor", "standard.vs", NULL, NULL, NULL, "flatcolor.fs");
  texturedShader = newShader("textured", "standard.vs", NULL, NULL, NULL, "textured.fs");
  reflectShader = newShader("reflect", "standard.vs", NULL, NULL, NULL, "reflect.fs");
  billboardShader = newShader("billboard", "billboard.vs", NULL, NULL, NULL, "billboard.fs");

  solidShadow = newShader("solidshadow", "shadow.vs", NULL, NULL, NULL, "solidshadow.fs");
  textureShadow = newShader("textureshadow", "shadow.vs", NULL, NULL, NULL, "textureshadow.fs");
};

void unload_shaders() {
  if (textureShadow != NULL) {
    shaderRelease(textureShadow);
  };
  if (solidShadow != NULL) {
    shaderRelease(solidShadow);
  };
  if (billboardShader != NULL) {
    shaderRelease(billboardShader);
  };
  if (colorShader != NULL) {
    shaderRelease(colorShader);
  };
  if (texturedShader != NULL) {
    shaderRelease(texturedShader);
  };
  if (reflectShader != NULL) {
    shaderRelease(reflectShader);
  };
  if (skyboxShader != NULL) {
    shaderRelease(skyboxShader);    
  };
  if (hmapShader != NULL) {
    shaderRelease(hmapShader);    
  };
};

//////////////////////////////////////////////////////////
// Objects

float getHeight(float x, float y) {
  vec4 col = tmapGetPixel(heightMap, x / 50000.0, y / 50000.0);
  float h =(col.x * 1000.0);

//  errorlog(0, "At %f, %f color = %f, height = %f", x, y, col.x, h);

  return h;
};

void initHMap() {
  material *    mat;
  meshNode *    mnode;
  mesh3d *      mesh;

  // we're hanging on to this...
  heightMap = getTextureMapByFileName("heightfield.jpg", GL_LINEAR, GL_REPEAT, true);
  tmapRetain(heightMap);

  mat = newMaterial("hmap");                  // create a material for our heightmap
  matSetShader(mat, hmapShader);              // texture shader for now, we do not set a shadow shader
  matSetDiffuseMap(mat, getTextureMapByFileName("grass.jpg", GL_LINEAR, GL_REPEAT, false));
  matSetBumpMap(mat, heightMap);

  mesh = newMesh(102 * 102, 101 * 101 * 3 * 2);
  strcpy(mesh->name, "hmap");
  meshSetMaterial(mesh, mat);
  meshMakePlane(mesh, 101, 101, 101.0, 101.0, maxPatches >= 4 ? 4 : 3);
  meshCopyToGL(mesh, true);

  // now add our heightfield to our scene
  mnode = newMeshNode("hmap");
  meshNodeSetMesh(mnode, mesh);
  meshNodeAddChild(scene, mnode);

  // we can release these seeing its now all contained within our scene
  matRelease(mat);
  meshRelease(mesh);
};

void initSkybox() {
  material *    mat;
  meshNode *    mnode;
  mesh3d *      mesh;

  mat = newMaterial("skybox");                // create a material for our skybox
  matSetShader(mat, skyboxShader);            // use our skybox shader, this will cause our lighting and positioning to be ignored!!!
  // we also do not set a shadow shader
  matSetDiffuseMap(mat, getTextureMapByFileName("skybox.png", GL_LINEAR, GL_CLAMP_TO_EDGE, false)); // load our texture map (courtesy of http://rbwhitaker.wikidot.com/texture-library)
 
  mesh = newMesh(24, 36);                     // init our cube with enough space for our buffers
  strcpy(mesh->name,"skybox");                // set name to cube
  meshSetMaterial(mesh, mat);                 // assign our material
  meshMakeCube(mesh, 100000.0, 100000.0, 100000.0, true, 3);  // create our cube, we make it as large as possible
  meshFlipFaces(mesh);                        // turn the mesh inside out
  meshCopyToGL(mesh, true);                   // copy our cube data to the GPU

  // now add our skybox to our scene
  mnode = newMeshNode("skybox");
  meshNodeSetMesh(mnode, mesh);
  meshNodeAddChild(scene, mnode);

  // we can release these seeing its now all contained within our scene
  matRelease(mat);
  meshRelease(mesh);
};

void addTieBombers(const char *pModelPath) {
  char *        text;
  vec3          tmpvector;
  mat4          adjust;

  // load our tie-bomber obj file
  text = loadFile(pModelPath, "tie-bomber.obj");
  if (text != NULL) {
    llist *       meshes = newMeshList();

    // setup our adjustment matrix to center our object
    mat4Identity(&adjust);
    mat4Translate(&adjust, vec3Set(&tmpvector, 250.0, -100.0, 100.0));

    // parse our object file
    meshParseObj(text, meshes, materials, &adjust);

    // add our tie bomber mesh to our containing node
    tieNodes[0] = newMeshNode("tie-bomber-0");
    mat4Translate(&tieNodes[0]->position, vec3Set(&tmpvector, 0.0, 1500.0, 0.0));
    meshNodeAddChildren(tieNodes[0], meshes);

    // create a bounding box for our tie-bomber
    meshNodeMakeBounds(tieNodes[0]);

    // and add it to our scene, note that we could free up our tie-bomber node here as it is references by our scene
    // but we keep it so we can interact with them.
    meshNodeAddChild(scene, tieNodes[0]);

    // and free up what we no longer need
    llistFree(meshes);
    free(text);

    // instance our tie bomber a few times, note that we're copying our tieNodes[0] model matrix as well so all our other bombers will be placed relative to it.
    tieNodes[1] = newCopyMeshNode("tie-bomber-1", tieNodes[0], false);
    mat4Translate(&tieNodes[1]->position, vec3Set(&tmpvector, -400.0, 0.0, -100.0));
    meshNodeAddChild(scene, tieNodes[1]);

    tieNodes[2] = newCopyMeshNode("tie-bomber-2", tieNodes[0], false);
    mat4Translate(&tieNodes[2]->position, vec3Set(&tmpvector, 400.0, 0.0, -100.0));
    meshNodeAddChild(scene, tieNodes[2]);

    tieNodes[3] = newCopyMeshNode("tie-bomber-3", tieNodes[0], false);
    mat4Translate(&tieNodes[3]->position, vec3Set(&tmpvector, 0.0, 0.0, 500.0));
    meshNodeAddChild(scene, tieNodes[3]);

    tieNodes[4] = newCopyMeshNode("tie-bomber-4", tieNodes[0], false);
    mat4Translate(&tieNodes[4]->position, vec3Set(&tmpvector, -600.0, 0.0, -400.0));
    meshNodeAddChild(scene, tieNodes[4]);

    tieNodes[5] = newCopyMeshNode("tie-bomber-5", tieNodes[0], false);
    mat4Translate(&tieNodes[5]->position, vec3Set(&tmpvector, 600.0, 0.0, -400.0));
    meshNodeAddChild(scene, tieNodes[5]);

    tieNodes[6] = newCopyMeshNode("tie-bomber-6", tieNodes[0], false);
    mat4Translate(&tieNodes[6]->position, vec3Set(&tmpvector, 0.0, 0.0, -500.0));
    meshNodeAddChild(scene, tieNodes[6]);

    tieNodes[7] = newCopyMeshNode("tie-bomber-7", tieNodes[0], false);
    mat4Translate(&tieNodes[7]->position, vec3Set(&tmpvector, -800.0, 0.0, -800.0));
    meshNodeAddChild(scene, tieNodes[7]);

    tieNodes[8] = newCopyMeshNode("tie-bomber-8", tieNodes[0], false);
    mat4Translate(&tieNodes[8]->position, vec3Set(&tmpvector, 800.0, 0.0, -800.0));
    meshNodeAddChild(scene, tieNodes[8]);

    tieNodes[9] = newCopyMeshNode("tie-bomber-9", tieNodes[0], false);
    mat4Translate(&tieNodes[9]->position, vec3Set(&tmpvector, 0.0, 0.0, -1000.0));
    meshNodeAddChild(scene, tieNodes[9]);
  };
};

float randomF(float pMin, float pMax) {
  float r = (float) rand();
  float m = (float) RAND_MAX;

  r /= m;
  r *= (pMax - pMin);
  r += pMin;

  return r;
};

void addTrees(const char *pModelPath) {
  char *        text;
  vec3          tmpvector;
  mat4          adjust;
  meshNode *    treeLod1 = NULL;
  meshNode *    treeLod2 = NULL;
  meshNode *    treeLod3 = NULL;
  meshNode *    treeGroups[101][101];
  int           i, j, tree;

  // zero out our tree groups
  memset(treeGroups, 0, sizeof(treeGroups));

  // load our tree obj files
  text = loadFile(pModelPath, "TreeLOD1.obj");
  if (text != NULL) {
    llist *       meshes = newMeshList();

    // just scale it up a bit
    mat4Identity(&adjust);
    mat4Scale(&adjust, vec3Set(&tmpvector, 40.0, 40.0, 40.0));

    // parse our object file
    meshParseObj(text, meshes, materials, &adjust);

    // and package as a tree node
    treeLod1 = newMeshNode("treeLod1");
    treeLod1->maxDist = 5000.0;
    meshNodeAddChildren(treeLod1, meshes);
    meshNodeMakeBounds(treeLod1);

    // and free up what we no longer need
    llistFree(meshes);
    free(text);
  };

  text = loadFile(pModelPath, "TreeLOD2.obj");
  if (text != NULL) {
    llist *       meshes = newMeshList();

    // just scale it up a bit
    mat4Identity(&adjust);
    mat4Scale(&adjust, vec3Set(&tmpvector, 40.0, 40.0, 40.0));

    // parse our object file
    meshParseObj(text, meshes, materials, &adjust);

    // and package as a tree node
    treeLod2 = newMeshNode("treeLod2");
    treeLod2->maxDist = 15000.0;
    meshNodeAddChildren(treeLod2, meshes); 
    meshNodeMakeBounds(treeLod2);

    // and free up what we no longer need
    llistFree(meshes);
    free(text);
  };

  // the last LOD we'll create by rendering to texture
  if (treeLod2 != NULL) {
    mesh3d *      mesh;
    material *    mat;
    texturemap *  tmap;
    vec3          normal, eye, lookat, upvector;
    vec2          t;

    // create our texture
    tmap = newTextureMap("treeLod3");
    tmapLoadData(tmap, NULL, 1024, 1024, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    if (tmapRenderToTexture(tmap, true)) {
      shaderMatrices  matrices;
      lightSource     light;
      mat4            tmpmatrix;

      // set our viewport
      glViewport(0,0,1024,1024);

      // clear our texture
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);      

      // setup our lightsource
      vec3Set(&light.position, 0.0, 1000000.0, 0.0);
      vec3Set(&light.adjPosition, 0.0, 1000000.0, 0.0);
      light.ambient = 1.0; // all ambient = no lighting

      // setup matrices
      mat4Identity(&tmpmatrix);
      mat4Ortho(&tmpmatrix, -500.0, 500.0, 1000.0, 0.0, 1000.0f, -1000.0f);
      shdMatSetProjection(&matrices, &tmpmatrix);

      mat4Identity(&tmpmatrix);
      mat4LookAt(&view, vec3Set(&eye, 0.0, 500.0, 1000.0), vec3Set(&lookat, 0.0, 500.0, 0.0), vec3Set(&upvector, 0.0, 1.0, 0.0));  
      shdMatSetView(&matrices, &tmpmatrix);

      // and render our tree to our texture
      meshNodeRender(treeLod2, &matrices, (material *) materials->first->data, &light);   

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      tmapFreeFrameBuffers(tmap);
    };

    // create our material
    mat = newMaterial("treeLod3");
    matSetShader(mat, billboardShader); // set our billboard shader, we do not set a shadow shader
    matSetDiffuseMap(mat, tmap);
    mat->shininess = 0.0;

    // create our mesh
    mesh = newMesh(4,2);
    meshSetMaterial(mesh, mat);
    vec3Set(&normal, 0.0, 0.0, 1.0);
    meshAddVNT(mesh, vec3Set(&tmpvector, -500.0, 1000.0, 0.0), &normal, vec2Set(&t, 0.0, 0.0));
    meshAddVNT(mesh, vec3Set(&tmpvector,  500.0, 1000.0, 0.0), &normal, vec2Set(&t, 1.0, 0.0));
    meshAddVNT(mesh, vec3Set(&tmpvector,  500.0,    0.0, 0.0), &normal, vec2Set(&t, 1.0, 1.0));
    meshAddVNT(mesh, vec3Set(&tmpvector, -500.0,    0.0, 0.0), &normal, vec2Set(&t, 0.0, 1.0));
    meshAddFace(mesh, 0, 1, 2);
    meshAddFace(mesh, 0, 2, 3);
    meshCopyToGL(mesh, true);

    treeLod3 = newMeshNode("treeLod3");
    treeLod3->maxDist = 30000.0;
    meshNodeSetMesh(treeLod3, mesh);
    // no point in adding bounds to LOD3, our shape is simpler then the box!

    // cleanup
    matRelease(mat);
    meshRelease(mesh);
    tmapRelease(tmap);
  };

  // add some trees
  for (tree = 0; tree < 5000; tree++) {
    meshNode * treeNode;
    char       nodeName[100];

    // create our node
    sprintf(nodeName, "tree_%d", tree);
    treeNode = newMeshNode(nodeName);
    treeNode->firstVisOnly = true; // only render the highest LOD

    // add our subnodes
    if (treeLod1 != NULL) {
      meshNodeAddChild(treeNode, treeLod1);
    };
    if (treeLod2 != NULL) {
      meshNodeAddChild(treeNode, treeLod2);
    };
    if (treeLod3 != NULL) {
      meshNodeAddChild(treeNode, treeLod3);
    };

    // position our node
    tmpvector.x = randomF(-50000.0, 50000.0);
    tmpvector.z = randomF(-50000.0, 50000.0);
    tmpvector.y = getHeight(tmpvector.x, tmpvector.z) - 15.0;
    mat4Translate(&treeNode->position, &tmpvector);

    // and add to our scene
    // meshNodeAddChild(scene, treeNode);

    // add to our tree groups
    i = (tmpvector.x + 50000.0) / 5000.0;
    j = (tmpvector.z + 50000.0) / 5000.0;
    if (treeGroups[i][j] == NULL) {
      sprintf(nodeName, "treeGroup_%d_%d", i, j);
      treeGroups[i][j] = newMeshNode(nodeName);

      // position our node
      tmpvector.x = (5000.0 * i) - 50000.0;
      tmpvector.z = (5000.0 * j) - 50000.0;
      tmpvector.y = getHeight(tmpvector.x, tmpvector.z) - 15.0;
      mat4Translate(&treeGroups[i][j]->position, &tmpvector);

      // set a maximum distance as we wouldn't be rendering any trees if it's this far away
      treeGroups[i][j]->maxDist = 35000.0;

      // and add to our scene
      meshNodeAddChild(scene, treeGroups[i][j]);
    };

    // and now reposition our tree and add to our group
    treeNode->position.m[3][0] -= treeGroups[i][j]->position.m[3][0];
    treeNode->position.m[3][1] -= treeGroups[i][j]->position.m[3][1];
    treeNode->position.m[3][2] -= treeGroups[i][j]->position.m[3][2];
    meshNodeAddChild(treeGroups[i][j], treeNode);

    // and we no longer need this
    meshNodeRelease(treeNode);
  };

  // free our trees, we don't need to hang on to it anymore
  if (treeLod1 != NULL) {
    meshNodeRelease(treeLod1);
  };
  if (treeLod2 != NULL) {
    meshNodeRelease(treeLod2);
  };
  if (treeLod3 != NULL) {
    meshNodeRelease(treeLod3);
  };
  for (j = 0; j < 101; j++) {
    for (i = 0; i < 101; i++) {
      if (treeGroups[i][j] != NULL) {
        meshNodeMakeBounds(treeGroups[i][j]);

        meshNodeRelease(treeGroups[i][j]);
      };
    };
  };
};

void addShadowMapObj() {
  material *    mat;
  mesh3d *      mesh;
  meshNode *    node;
  vec3          normal, tmpvector;
  vec2          t;

  // create our material
  mat = newMaterial("test");
  matSetShader(mat, billboardShader); // set our billboard shader, we do not set a shadow shader
  matSetDiffuseMap(mat, sun.shadowMap);
  mat->shininess = 0.0;

  // create our mesh
  mesh = newMesh(4,2);
  meshSetMaterial(mesh, mat);
  vec3Set(&normal, 0.0, 0.0, 1.0);
  meshAddVNT(mesh, vec3Set(&tmpvector, -500.0, 1000.0, 0.0), &normal, vec2Set(&t, 0.0, 0.0));
  meshAddVNT(mesh, vec3Set(&tmpvector,  500.0, 1000.0, 0.0), &normal, vec2Set(&t, 1.0, 0.0));
  meshAddVNT(mesh, vec3Set(&tmpvector,  500.0,    0.0, 0.0), &normal, vec2Set(&t, 1.0, 1.0));
  meshAddVNT(mesh, vec3Set(&tmpvector, -500.0,    0.0, 0.0), &normal, vec2Set(&t, 0.0, 1.0));
  meshAddFace(mesh, 0, 1, 2);
  meshAddFace(mesh, 0, 2, 3);
  meshCopyToGL(mesh, true);

  node = newMeshNode("test");
  meshNodeSetMesh(node, mesh);

  // position our node
  tmpvector.x = 0.0;
  tmpvector.z = 0.0;
  tmpvector.y = getHeight(tmpvector.x, tmpvector.z) - 15.0;
  mat4Translate(&node->position, &tmpvector);

  meshNodeAddChild(scene, node);

  meshNodeRelease(node);
  meshRelease(mesh);
  matRelease(mat);
};

void load_objects() {
  char          modelPath[1024];
  char *        text;
  material *    mat;
  llistNode *   lnode;

  // init some paths
  #ifdef __APPLE__
    tmapSetTexturePath("Textures/");
    strcpy(modelPath,"Models/");
  #else
    tmapSetTexturePath("Resources\\Textures\\");
    strcpy(modelPath,"Resources\\Models\\");
  #endif

  // create a retainer for materials
  materials = newMatList();

  // create our default material, make sure it's the first one
  mat = newMaterial("Default");
  matSetShader(mat, colorShader);
  matSetShadowShader(mat, solidShadow);
  llistAddTo(materials, mat);
  matRelease(mat);
  mat = NULL;

  // load our mtl file(s)
  text = loadFile(modelPath,"tie-bomber.mtl");
  if (text != NULL) {
    matParseMtl(text, materials);
      
    free(text);
  };    
  text = loadFile(modelPath,"tree.mtl");
  if (text != NULL) {
    matParseMtl(text, materials);
      
    free(text);
  };
  // we render our leaves two sided
  mat = getMatByName(materials, "Leaves");
  if (mat != NULL) {
    mat->twoSided = true;
  };

  // assign shaders to our materials
  lnode = materials->first;
  while (lnode != NULL) {
    mat = (material * ) lnode->data;

    // assign both solid and shadow shaders, note that our shadow shader will be ignored for transparent shadows
    if (mat->reflectMap != NULL) {  
      matSetShader(mat, reflectShader);
      matSetShadowShader(mat, solidShadow);
    } else if (mat->diffuseMap != NULL) {          
      matSetShader(mat, texturedShader);
      matSetShadowShader(mat, textureShadow);
    } else {
      matSetShader(mat, colorShader);
      matSetShadowShader(mat, solidShadow);
    };
    
    lnode = lnode->next;
  };

  // create a material for rendering bounds (we moved this down because we do not want a shadow shader!)
  mat = newMaterial("Bounds");
  matSetShader(mat, colorShader);
  vec3Set(&mat->matColor, 0.0, 1.0, 0.0);
  mat->alpha = 0.5;
  mat->shininess = 0.0;
  llistAddTo(materials, mat);
  meshNodeSetBoundsDebugMaterial(mat);
  matRelease(mat);
  mat = NULL;

  // create our root node
  scene = newMeshNode("scene");
  if (scene != NULL) {
    // add our tie bombers
    addTieBombers(modelPath);
    
    // create our height map object
    initHMap();

    // add our trees
    addTrees(modelPath);

    // And create our skybox, we no longer add this to our scene so we can handle this last in our rendering loop
    initSkybox();

    // add test object to show our shadow map
    // addShadowMapObj();
  }; 
};

void unload_objects() {
  int i;

  // free our object data
  errorlog(0, "Unloading objects...");

  // release our tie-bomber nodes
  for (i = 0; i < 10; i++) {
    if (tieNodes[i] != NULL) {
      meshNodeRelease(tieNodes[i]);
      tieNodes[i] = NULL;
    };
  };

  if (scene != NULL) {
    meshNodeRelease(scene);
    scene = NULL;
  };
  
  if (materials != NULL) {
    llistFree(materials);
    materials = NULL;
  };

  if (heightMap != NULL) {
    tmapRelease(heightMap);
    heightMap = NULL;
  };

  // do this last just in case...
  tmapReleaseCachedTextureMaps();
};

//////////////////////////////////////////////////////////
// Main engine

// engineInit initializes any variables, kinda like our constructor
void engineInit() {

};

// engineLoad loads any data that we need to load before we can start outputting stuff
void engineLoad() {
  vec3  upvector;
  
  // load our font
  load_font();
    
  // load, compile and link our shader(s)
  load_shaders();
  
  // setup our light
  vec3Set(&sun.position, 100000.0, 100000.0, 0.00);
  sun.ambient = 0.3;
  sun.shadowMap = newTextureMap("shadowmap");
  mat4Identity(&sun.shadowMat);
  
  // load our objects
  load_objects();
  
  // init our view matrix
  mat4Identity(&view);
  mat4LookAt(&view, &camera_eye, &camera_lookat, vec3Set(&upvector, 0.0, 1.0, 0.0));  
};

// engineUnload unloads and frees up any data associated with our engine
void engineUnload() {
  // lets be nice and cleanup
  if (sun.shadowMap != NULL) {
    tmapRelease(sun.shadowMap);
    sun.shadowMap = NULL;
  };

  unload_shaders();
  unload_objects();
  unload_font();
};

// engineUpdate is called to handle any updates of our data
// pSecondsPassed is the number of seconds passed since our application was started.
void engineUpdate(double pSecondsPassed) {
  const joystickInfo * joystick = getJoystickInfo(0);
  float         delta;
  vec3          avector, bvector, upvector;
  mat4          M;
  float         moveHorz = 0.0;
  float         moveVert = 0.0;
  float         moveForward = 0.0;
  float         moveSideways = 0.0;
  float         height;
  
  // handle our joystick
  if (joystick == NULL) {
    // no joystick
  } else if (joystick->enabled) {
    moveHorz = joystick->axes[0] * 2.0;
    moveVert = joystick->axes[1] * 2.0;
    moveForward = joystick->axes[3] * 20.0;
    moveSideways = joystick->axes[2] * 20.0;
  };
    
  // handle our keys....
  if (engineKeyPressedCallback(GLFW_KEY_A)) {
    moveHorz = 1.0;
  } else if (engineKeyPressedCallback(GLFW_KEY_D)) {
    moveHorz = -1.0;
  };
  if (engineKeyPressedCallback(GLFW_KEY_W)) {
    moveVert = 1.0;
  } else if (engineKeyPressedCallback(GLFW_KEY_S)) {
    moveVert = -1.0;
  };
  if (engineKeyPressedCallback(GLFW_KEY_Z)) {
    moveForward = -10.0;
  } else if (engineKeyPressedCallback(GLFW_KEY_C)) {
    moveForward = 10.0;
  };
    
  // adjust camera
  if ((moveHorz <= -0.1) || (moveHorz >= 0.1)) {
    // rotate position left
    
    // get our (reverse) looking direction vector
    vec3Copy(&avector, &camera_eye);
    vec3Sub(&avector, &camera_lookat);
    
    // rotate our looking direction vector around our up vector
    mat4Identity(&M);
    mat4Rotate(&M, moveHorz, vec3Set(&bvector, view.m[0][1], view.m[1][1], view.m[2][1]));
    
    // and update our eye position accordingly
    mat4ApplyToVec3(&camera_eye, &avector, &M);
    vec3Add(&camera_eye, &camera_lookat);
  };

  if ((moveVert <= -0.1) || (moveVert >= 0.1)) {
    // get our (reverse) looking direction vector
    vec3Copy(&avector, &camera_eye);
    vec3Sub(&avector, &camera_lookat);
    
    // rotate our looking direction vector around our right vector
    mat4Identity(&M);
    mat4Rotate(&M, moveVert, vec3Set(&bvector, view.m[0][0], view.m[1][0], view.m[2][0]));
    
    // and update our eye position accordingly
    mat4ApplyToVec3(&camera_eye, &avector, &M);
    vec3Add(&camera_eye, &camera_lookat);
  };

  if ((moveForward <= -0.1) || (moveForward >= 0.1)) {
    vec3Set(&avector, view.m[0][2], view.m[1][2], view.m[2][2]); // our forward vector
    vec3Scale(&avector, moveForward);

    vec3Add(&camera_eye, &avector);
    vec3Add(&camera_lookat, &avector);
  };

  if ((moveSideways <= -0.1) || (moveSideways >= 0.1)) {
    vec3Set(&avector, view.m[0][0], view.m[1][0], view.m[2][0]); // our right vector
    vec3Scale(&avector, moveSideways);

    vec3Add(&camera_eye, &avector);
    vec3Add(&camera_lookat, &avector);
  };

  // get our height at the camera position
  height = 30.0 + getHeight(camera_eye.x, camera_eye.z);
  if (height > camera_eye.y) {
    height -= camera_eye.y;
    camera_eye.y += height;
    camera_lookat.y += height;
  };

  // update our view matrix
  mat4Identity(&view);
  mat4LookAt(&view, &camera_eye, &camera_lookat, vec3Set(&upvector, 0.0, 1.0, 0.0));
    
  // update our frame counter
  frames += 1.0f;
  delta = pSecondsPassed - lastsecs;
  fps = (frames - lastframes) / delta;

  if (frames - lastframes > 100.0f) {
    // reset every 100 frames
    lastsecs = pSecondsPassed;
    lastframes = frames;
  };
};

// render our shadow map
// we'll place this in our engine.h for now but we'll soon make this part of our lighting library
void renderShadowMapForSun() {
  // we'll initialize a 4096x4096 shadow map for our sun
  if (tmapRenderToShadowMap(sun.shadowMap, 4096, 4096)) {
    mat4            tmpmatrix;
    vec3            tmpvector;
    shaderMatrices  matrices;
    GLint           wasviewport[4];

    // remember our current viewport
    glGetIntegerv(GL_VIEWPORT, &wasviewport[0]);

    // set our viewport
    glViewport(0, 0, 4096, 4096);

    // clear our depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);      

    // enable and configure our backface culling, note that here we cull our front facing polygons
    // to minimize shading artifacts
    glEnable(GL_CULL_FACE);   // enable culling
    glFrontFace(GL_CW);       // clockwise
    glCullFace(GL_FRONT);     // frontface culling

    // enable our depth test
    glEnable(GL_DEPTH_TEST);
    // disable alpha blending  
    glDisable(GL_BLEND);
    // solid polygons
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    

    // need to create our projection matrix first
    // for our sun we need an orthographic projection as rays of sunlight pretty much are parallel to each other.
    // if this was a spotlight a perspective projection gives the best result
    mat4Identity(&tmpmatrix);
    mat4Ortho(&tmpmatrix, -10000.0, 10000.0, -10000.0, 10000.0, -50000.0, 50000.0);
//    mat4Ortho(&tmpmatrix, -1000.0, 1000.0, -1000.0, 1000.0, -50000.0, 50000.0);
    shdMatSetProjection(&matrices, &tmpmatrix);

    // We are going to adjust our sun's position based on our camera position.
    // We position the sun such that our camera location would be at Z = 0.
    // Our near plane is actually behind our 'sun' which gives us some wiggleroom.
    vec3Copy(&sun.adjPosition, &sun.position);
    vec3Normalise(&sun.adjPosition);  // normalize our sun position vector
    vec3Mult(&sun.adjPosition, 10000.0); // move the sun far enough away
    vec3Add(&sun.adjPosition, &camera_eye); // position in relation to our camera

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
    mat4LookAt(&tmpmatrix, &sun.adjPosition, &camera_eye, vec3Set(&tmpvector, 0.0, 0.0, 1.0));
    shdMatSetView(&matrices, &tmpmatrix);

    // now we override our eye position to be at our camera position, this is important for our LOD calculations
    shdMatSetEyePos(&matrices, &camera_eye);

    // now remember our view-projection matrix, we need it later on when rendering our scene
    mat4Copy(&sun.shadowMat, shdMatGetViewProjection(&matrices));

    // and now render our scene for shadow maps (note that we only render materials that have a shadow shader and we ignore transparent objects)
    if (scene != NULL) {
      meshNodeShadowMap(scene, &matrices);    
    };

    // and output back to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(wasviewport[0],wasviewport[1],wasviewport[2],wasviewport[3]);
  };
};

// engineRender is called to render our stuff
// pWidth and pHeight define the size of our drawing buffer
// pMode is 0 => mono, 1 => left eye, 2 => right eye
void engineRender(int pWidth, int pHeight, float pRatio, int pMode) {
  mat4            tmpmatrix;
  shaderMatrices  matrices;
  vec3            tmpvector;
  float           left, top;
  int             i;

  // only render our shadow maps once per frame, we can reuse them if we're doing our right eye as well
  if (pMode != 2) {
    renderShadowMapForSun();
  };

  // calculate our sun position, we want to do this only once
  mat4ApplyToVec3(&sun.adjPosition, &sun.position, &view);
        
  // enable and configure our backface culling
  glEnable(GL_CULL_FACE);   // enable culling
  glFrontFace(GL_CW);       // clockwise
  glCullFace(GL_BACK);      // backface culling

  // enable our depth test
  glEnable(GL_DEPTH_TEST);
  // disable alpha blending  
  glDisable(GL_BLEND);

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  };

  // init our projection matrix, we use a 3D projection matrix now
  mat4Identity(&tmpmatrix);
  // distance between eyes is on average 6.5 cm, this should be setable
  mat4Stereo(&tmpmatrix, 45.0, pRatio, 1.0, 100000.0, 6.5, 200.0, pMode);
//  mat4Ortho(&tmpmatrix, -15000.0, 15000.0, -15000.0, 15000.0, -50000.0, 50000.0);
  shdMatSetProjection(&matrices, &tmpmatrix); // call our set function to reset our flags
  
  // copy our view matrix into our state
  shdMatSetView(&matrices, &view);
  
  // and render our scene
  if (scene != NULL) {
    meshNodeRender(scene, &matrices, (material *) materials->first->data, &sun);    

    // This may have been turned on in our node renderer
    glDisable(GL_BLEND);
  };
  
  // unset stuff
  glBindVertexArray(0);
  glUseProgram(0);
  
  if (showinfo) {
    ////////////////////////////////////
    // UI
      
    // change our state a little
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
  	glEnable(GL_BLEND);
  	glBlendEquation(GL_FUNC_ADD);
  	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   
    // now render our FPS
  	if ((fs != NULL) && (font != FONS_INVALID)) {
      const joystickInfo * joystick = getJoystickInfo(0);
      float virtualScreenHeight = 250.0;
      char info[256];
      
      // We want a orthographic projection for our frame counter
      mat4Identity(&tmpmatrix);
      mat4Ortho(&tmpmatrix, -pRatio * virtualScreenHeight, pRatio * virtualScreenHeight, virtualScreenHeight, -virtualScreenHeight, 1.0f, -1.0f);
      // shdMatSetProjection(&matrices, &tmpmatrix);
      gl3fonsProjection(fs, (GLfloat *)tmpmatrix.m);

      // what text shall we draw?
      sprintf(info,"FPS: %0.1f, use wasd to rotate the camera, zc to move forwards/backwards. f to toggle wireframe", fps);
      fonsDrawText(fs, -pRatio * 250.0f, 230.0f, info, NULL);
      
      // lets display some info about our joystick:
      if (joystick != NULL) {
        if (joystick->enabled) {
          sprintf(info, "Joystick %s is active", joystick->name);
          fonsDrawText(fs, -pRatio * 250.0f, -250.0f, info, NULL);

          sprintf(info, "Axes: %f %f %f %f %f %f %f %f", joystick->axes[0], joystick->axes[1], joystick->axes[2], joystick->axes[3], joystick->axes[4], joystick->axes[5], joystick->axes[6], joystick->axes[7]);
          fonsDrawText(fs, -pRatio * 250.0f, -230.0f, info, NULL);

          sprintf(info, "Buttons: %i %i %i %i %i %i %i %i", joystick->buttons[0], joystick->buttons[1], joystick->buttons[2], joystick->buttons[3], joystick->buttons[4], joystick->buttons[5], joystick->buttons[6], joystick->buttons[7]);
          fonsDrawText(fs, -pRatio * 250.0f, -210.0f, info, NULL);        
        } else {
          sprintf(info, "Joystick %s is inactive", joystick->name);
          fonsDrawText(fs, -pRatio * 250.0f, -250.0f, info, NULL);
        };
      };
    };
  };
};

void engineKeyPressed(int pKey) {
//  pushKey(pKey);
  if (pKey == GLFW_KEY_F) {
    // toggle wireframe
    wireframe = !wireframe;
  } else if (pKey == GLFW_KEY_I) {
    // toggle info
    showinfo = !showinfo;
  } else if (pKey == GLFW_KEY_B) {
    bounds = !bounds;
    meshNodeSetRenderBounds(bounds);
  };
};
