/********************************************************
 * math3d.h - 3D math library by Bastiaan Olij 2015
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define MATH3D_IMPLEMENTATION
 *
 * Note that by default this library uses GLfloat and you
 * thus need to include GL headers before including this
 * file. 
 * If you wish to use normal floats (or other type)
 * proceed the include with:
 * #define MATH3D_FLOAT float
 * but you must do this consistently
 *
 * This library is written in C but follows an object 
 * pattern where we use a struct to define the data 
 * members of an object and send a pointer to that
 * object to each method that works on/with that object.
 * Many functions will return the pointer to the object
 * to allow further functions to be executed on the
 * object.
 * 
 * This library currently implements a 2D, 3D and 4D
 * vector 'class' and a 3x3 and 4x4 matrix class.
 * A vector is simply a directional pointer.
 * A matrix is a construct that allows us to apply
 * complex transformations to a vector.
 *
 * Sometimes we mention vertices and in a 3D engine
 * they are often represented by a vector. So what is
 * the difference between a vector and a vertex?
 * A vertex is a point in space.
 * You could think of one being an absolute (vertex)
 * and the other a relative (vector) variable.
 * However a vertex is often much more as it not only
 * defines a position in space but often has properties
 * such as a normal and one or more texture coordinates.
 *
 * Finally, why all the pointers? Why not just normal
 * parameters. It doesn't seem to make sense.
 * Keep in mind that we're using floats here and some
 * of these structures are relatively large.
 * If you decide to compile this using doubles.....
 * Using pointers prevents unneedingly copying around
 * data all the time. It does mean you need to define
 * an extra variable on the stack sometimes.
 *
 * Revision history:
 * 0.1  07-03-2015  First version with basic functions
 * 0.2  31-01-2016  Fixed inverse matrix function
 * 0.3  23-02-2016  Added vec4Mult and vec4Div 
 *
 ********************************************************/

#ifndef math3dh
#define math3dh

// standard libraries we need...
#include <stdbool.h>
#include <math.h>

#ifndef MATH3D_FLOAT
#define MATH3D_FLOAT GLfloat
#endif /* !MATH3D_FLOAT */

#ifndef MATH3D_EPSILON
// need to set this correctly!
#define MATH3D_EPSILON 0.0000001
#endif /* !MATH3D_EPSILON */

// we need PI...
#ifndef PI
#define PI 3.14159265358979323846264
#endif /* !PI */

#define angToRad(a) a * PI / 180.0
#define radToAng(r) r * 180.0 / PI

// 2D vector
typedef struct vec2 {
  MATH3D_FLOAT x;
  MATH3D_FLOAT y;
} vec2;

// 3D vector
typedef struct vec3 {
  MATH3D_FLOAT x;
  MATH3D_FLOAT y;
  MATH3D_FLOAT z;
} vec3;

// 4D vector
typedef struct vec4 {
  MATH3D_FLOAT x;
  MATH3D_FLOAT y;
  MATH3D_FLOAT z;
  MATH3D_FLOAT w;
} vec4;

// 3x3 matrix
typedef struct mat3 {
  MATH3D_FLOAT m[3][3];
} mat3;

// 4x4 matrix
typedef struct mat4 {
  MATH3D_FLOAT m[4][4];
} mat4;

#ifdef __cplusplus
extern "C" {
#endif

// vec2 interface
vec2* vec2Set(vec2* pSet, MATH3D_FLOAT pX, MATH3D_FLOAT pY);
vec2* vec2Copy(vec2* pSet, const vec2* pCopy);
MATH3D_FLOAT vec2Dot(const vec2* pVecA, const vec2* pVecB);
MATH3D_FLOAT vec2Lenght(const vec2* pLengthOf);
vec2* vec2Normalise(vec2 *pNormalise);
vec2* vec2Add(vec2* pAddTo, const vec2* pAdd);
vec2* vec2Sub(vec2* pSubFrom, const vec2* pSub);
vec2* vec2Mult(vec2* pVec, const float pMult);
vec2* vec2Div(vec2* pVec, const float pDiv);
vec2* vec2Scale(vec2* pSet, const float pScale);

// vec3 interface
vec3* vec3Set(vec3* pSet, MATH3D_FLOAT pX, MATH3D_FLOAT pY, MATH3D_FLOAT pZ);
vec3* vec3Copy(vec3* pSet, const vec3* pCopy);
MATH3D_FLOAT vec3Dot(const vec3* pVecA, const vec3* pVecB);
vec3* vec3Cross(vec3* pDest, const vec3* pVecA, const vec3* pVecB);
MATH3D_FLOAT vec3Lenght(const vec3* pLengthOf);
vec3* vec3Normalise(vec3 * pNormalise);
vec3* vec3Add(vec3* pAddTo, const vec3* pAdd);
vec3* vec3Sub(vec3* pSubFrom, const vec3* pSub);
vec3* vec3Mult(vec3* pVec, const float pMult);
vec3* vec3Div(vec3* pVec, const float pDiv);
vec3* vec3Scale(vec3* pSet, const float pScale);

// vec4 interface
vec4* vec4Set(vec4* pSet, MATH3D_FLOAT pX, MATH3D_FLOAT pY, MATH3D_FLOAT pZ, MATH3D_FLOAT pW);
vec4* vec4Copy(vec4* pSet, const vec4* pCopy);
MATH3D_FLOAT vec4Dot(const vec4* pVecA, const vec4* pVecB);
MATH3D_FLOAT vec4Lenght(const vec4* pLengthOf);
vec4* vec4Add(vec4* pAddTo, const vec4* pAdd);
vec4* vec4Sub(vec4* pSubFrom, const vec4* pSub);
vec4* vec4Mult(vec4* pVec, const float pMult);
vec4* vec4Div(vec4* pVec, const float pDiv);
vec4* vec4Scale(vec4* pSet, const float pScale);

vec3* vec3FromVec4(vec3* pSet, const vec4* pFrom, bool pApplyW);
vec4* vec4FromVec3(vec4* pSet, const vec3* pFrom, MATH3D_FLOAT pW);

// mat3 interface
mat3* mat3Copy(mat3* pDest, const mat3* pSource);
mat3* mat3Identity(mat3* pSet);
mat3* mat3Transpose(mat3* pTranspose);
vec3* mat3ApplyToVec3(vec3* pSet, const vec3* pApplyTo, const mat3* pMatrix);
mat3* mat3Multiply(mat3* pMultTo, const mat3* pMultWith);
mat3* mat3Rotate(mat3* pMatrix, MATH3D_FLOAT pAngle, const vec3* pAxis);
mat3* mat3Scale(mat3 *pMatrix, const vec3* pScale);

// mat4 interface
mat4* mat4Copy(mat4* pDest, const mat4* pSource);
mat4* mat4Identity(mat4* pSet);
mat4* mat4Transpose(mat4* pTranspose);
vec3* mat4ApplyToVec3(vec3* pSet, const vec3* pApplyTo, const mat4* pMatrix);
vec4* mat4ApplyToVec4(vec4* pSet, const vec4* pApplyTo, const mat4* pMatrix);
mat4* mat4Multiply(mat4* pMultTo, const mat4* pMultWith);
mat4* mat4Inverse(mat4* pInverse, const mat4* pMatrix);
mat4* mat4Rotate(mat4* pMatrix, MATH3D_FLOAT pAngle, const vec3* pAxis);
mat4* mat4Translate(mat4 *pMatrix, const vec3* pAxis);
mat4* mat4Scale(mat4 *pMatrix, const vec3* pScale);
mat4* mat4Ortho(mat4* pMatrix, MATH3D_FLOAT pLeft, MATH3D_FLOAT pRight, MATH3D_FLOAT pBottom, MATH3D_FLOAT pTop, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar);
mat4* mat4Frustum(mat4* pMatrix, MATH3D_FLOAT pLeft, MATH3D_FLOAT pRight, MATH3D_FLOAT pBottom, MATH3D_FLOAT pTop, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar);
mat4* mat4Projection(mat4* pMatrix, MATH3D_FLOAT pFOV, MATH3D_FLOAT pAspect, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar);
mat4* mat4Stereo(mat4* pMatrix, MATH3D_FLOAT pFOV, MATH3D_FLOAT pAspect, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar, float pIOD, float pProjPlane, int pMode);
mat4* mat4LookAt(mat4* pMatrix, const vec3* pEye, const vec3* pLookat, const vec3* pUp);

mat3* mat3FromMat4(mat3* pSet, const mat4* pFrom);
mat4* mat4FromMat3(mat4* pSet, const mat3* pFrom);

#ifdef __cplusplus
};
#endif  

#ifdef MATH3D_IMPLEMENTATION

////////////////////////////////////////////////////////////////////////////////////
// vec2

// Initialize a 2D vector with values:
// vec2 vector;
// vec2Set(&vector, 0.0, 0.0);
// vector is now (0.0, 0.0)
vec2* vec2Set(vec2* pSet, MATH3D_FLOAT pX, MATH3D_FLOAT pY) {
  pSet->x = pX;
  pSet->y = pY;

  return pSet;
};

// copies a vec2 vector
vec2* vec2Copy(vec2* pSet, const vec2* pCopy) {
  pSet->x = pCopy->x;
  pSet->y = pCopy->y;

  return pSet;  
};

// Returns the dot product between two vectors. 
// vec2 vecA, vecB;
// vec2Set(&vecA, 0.0, 0.1);
// vec2Set(&vecB, 0.1, 0.0);
// MATH3D_FLOAT dot = vec2Dot(&vecA, &vecB);
// dot is now 0.0
// Note that when the two vectors are unit vectors (normalized) vec2Dot actually returns the cosine value of the angle between the two vectors. Amazingly handy.
MATH3D_FLOAT vec2Dot(const vec2* pVecA, const vec2* pVecB) {
  return (pVecA->x * pVecB->x) + (pVecA->y * pVecB->y);
};

// Normalises our vector, this results in our vector becoming unit lenght.
// vec2 vector;
// vec2Set(&vector, 5.0, 0.0);
// vec2Normalise(&vector);
// vector is now (1.0, 0.0)
vec2* vec2Normalise(vec2 *pNormalise) {
  MATH3D_FLOAT dot = vec2Dot(pNormalise, pNormalise);
  if (dot <= MATH3D_EPSILON) {
    // can't normalize a vector with 0.0 length  
    pNormalise->x = 1.0;
    pNormalise->y = 0.0;
  } else {
    dot = sqrt(dot);
    pNormalise->x /= dot;
    pNormalise->y /= dot;
  };
  
  return pNormalise;
};

// Returns the lenght of a vector
// vec2 vector;
// vec2Set(&vector, 0.0, 5.0);
// MATH3D_FLOAT L = vec2Length(&vector);
// L is now 5.0
MATH3D_FLOAT vec2Lenght(const vec2* pLengthOf) {
  return sqrt(vec2Dot(pLengthOf, pLengthOf));
};

// Adds a vector to another vector:
// vec2 vecA, vecB;
// vec2Set(&vecA, 0.0, 0.1);
// vec2Set(&vecB, 0.1, 0.0);
// vec2Add(&vecA, &vecB);
// vecA is now (1.0, 1.0)
vec2* vec2Add(vec2* pAddTo, const vec2* pAdd) {
  pAddTo->x += pAdd->x;
  pAddTo->y += pAdd->y;
  
  return pAddTo;
};

// Subtracts a vector from another vector
// vec2 vecA, vecB;
// vec2Set(&vecA, 0.0, 0.1);
// vec2Set(&vecB, 0.1, 0.0);
// vec2Sub(&vecA, &vecB);
// vecA is now (-1.0, 1.0)
vec2* vec2Sub(vec2* pSubFrom, const vec2* pSub) {
  pSubFrom->x -= pSub->x;
  pSubFrom->y -= pSub->y;
  
  return pSubFrom;
};

// Multiply a vector with a constante
vec2* vec2Mult(vec2* pVec, const float pMult) {
  pVec->x = pVec->x * pMult;
  pVec->y = pVec->y * pMult;
  
  return pVec;
};

// Divide a vector by a constante
vec2* vec2Div(vec2* pVec, const float pDiv) {
  if (pDiv != 0.0) {
    pVec->x = pVec->x / pDiv;
    pVec->y = pVec->y / pDiv;
  };
  
  return pVec;  
};

// Scale a vector
// vec2 vec;
// vec2Set(&vec, 1.0, 2.0);
// vec2Scale(&vec, 2.0);
// vec is now (2.0, 4.0)
vec2* vec2Scale(vec2* pSet, const float pScale) {
  pSet->x *= pScale;
  pSet->y *= pScale;
  
  return pSet;  
};

////////////////////////////////////////////////////////////////////////////////////
// vec3

// Initialize a 3D vector with values:
// vec3 vector;
// vec3Set(&vector, 0.0, 0.0, 0.0);
// vector is now (0.0, 0.0, 0.0)
vec3* vec3Set(vec3* pSet, MATH3D_FLOAT pX, MATH3D_FLOAT pY, MATH3D_FLOAT pZ) {
  pSet->x = pX;
  pSet->y = pY;
  pSet->z = pZ;

  return pSet;
};

// copies a vec3 vector
vec3* vec3Copy(vec3* pSet, const vec3* pCopy) {
  pSet->x = pCopy->x;
  pSet->y = pCopy->y;
  pSet->z = pCopy->z;

  return pSet;  
};

// Returns the dot product between two vectors. 
// vec3 vecA, vecB;
// vec3Set(&vecA, 0.0, 0.1, 0.0);
// vec3Set(&vecB, 0.1, 0.0, 1.0);
// MATH3D_FLOAT dot = vec3Dot(&vecA, &vecB);
// dot is now 0.0
// Note that when the two vectors are unit vectors (normalized) vec3Dot actually returns the cosine value of the angle between the two vectors. Amazingly handy.
MATH3D_FLOAT vec3Dot(const vec3* pVecA, const vec3* pVecB) {
  return (pVecA->x * pVecB->x) + (pVecA->y * pVecB->y) + (pVecA->z * pVecB->z);
};

// Returns the cross product between two vectors.
// When the two vectors are unit vectors the cross prodcut results in a vector that is perpendicular to the other two vectors.
vec3* vec3Cross(vec3* pDest, const vec3* pVecA, const vec3* pVecB) {
  pDest->x = (pVecA->y * pVecB->z) - (pVecA->z * pVecB->y);
  pDest->y = (pVecA->z * pVecB->x) - (pVecA->x * pVecB->z);
  pDest->z = (pVecA->x * pVecB->y) - (pVecA->y * pVecB->x);
  
  return pDest;
};

// Returns the lenght of a vector
// vec3 vector;
// vec3Set(&vector, 0.0, 5.0, 0.0);
// MATH3D_FLOAT L = vec3Length(&vector);
// L is now 5.0
MATH3D_FLOAT vec3Lenght(const vec3* pLengthOf) {
  return sqrt(vec3Dot(pLengthOf, pLengthOf));
};

// Normalises our vector, this results in our vector becoming unit lenght.
// vec3 vector;
// vec3Set(&vector, 5.0, 0.0, 0.0);
// vec3Normalise(&vector);
// vector is now (1.0, 0.0, 0.0)
vec3* vec3Normalise(vec3 *pNormalise) {
  MATH3D_FLOAT dot = vec3Dot(pNormalise, pNormalise);
  if (dot <= MATH3D_EPSILON) {
    // can't normalize a vector with 0.0 length  
    pNormalise->x = 1.0;
    pNormalise->y = 0.0;
    pNormalise->z = 0.0;
  } else {
    dot = sqrt(dot);
    pNormalise->x /= dot;
    pNormalise->y /= dot;
    pNormalise->z /= dot;
  };
  
  return pNormalise;
};

// Adds a vector to another vector:
// vec3 vecA, vecB;
// vec3Set(&vecA, 0.0, 0.1, 0.0);
// vec3Set(&vecB, 0.1, 0.0, 0.0);
// vec3Add(&vecA, &vecB);
// vecA is now (1.0, 1.0, 0.0)
vec3* vec3Add(vec3* pAddTo, const vec3* pAdd) {
  pAddTo->x += pAdd->x;
  pAddTo->y += pAdd->y;
  pAddTo->z += pAdd->z;
  
  return pAddTo;
};

// Subtracts a vector from another vector
// vec3 vecA, vecB;
// vec3Set(&vecA, 0.0, 0.1, 0.0);
// vec3Set(&vecB, 0.1, 0.0, 0.0);
// vec3Sub(&vecA, &vecB);
// vecA is now (-1.0, 1.0, 0.0)
vec3* vec3Sub(vec3* pSubFrom, const vec3* pSub) {
  pSubFrom->x -= pSub->x;
  pSubFrom->y -= pSub->y;
  pSubFrom->z -= pSub->z;
  
  return pSubFrom;
};

// Multiply a vector with a constante
vec3* vec3Mult(vec3* pVec, const float pMult) {
  pVec->x = pVec->x * pMult;
  pVec->y = pVec->y * pMult;
  pVec->z = pVec->z * pMult;
  
  return pVec;
};

// Divide a vector by a constante
vec3* vec3Div(vec3* pVec, const float pDiv) {
  if (pDiv != 0.0) {
    pVec->x = pVec->x / pDiv;
    pVec->y = pVec->y / pDiv;
    pVec->z = pVec->z / pDiv;    
  };
  
  return pVec;  
};

// Scale a vector
// vec3 vec;
// vec3Set(&vec, 1.0, 2.0, 3.0);
// vec3Scale(&vec, 2.0);
// vec is now (2.0, 4.0, 6.0)
vec3* vec3Scale(vec3* pSet, const float pScale) {
  pSet->x *= pScale;
  pSet->y *= pScale;
  pSet->z *= pScale;
  
  return pSet;  
};

////////////////////////////////////////////////////////////////////////////////////
// vec4

// Initialize a 4D vector with values:
// vec4 vector;
// vec4Set(&vector, 0.0, 0.0, 0.0, 0.0);
// vector is now (0.0, 0.0, 0.0, 0.0)
vec4* vec4Set(vec4* pSet, MATH3D_FLOAT pX, MATH3D_FLOAT pY, MATH3D_FLOAT pZ, MATH3D_FLOAT pW) {
  pSet->x = pX;
  pSet->y = pY;
  pSet->z = pZ;
  pSet->w = pW;

  return pSet;
};

// copies a vec4 vector
vec4* vec4Copy(vec4* pSet, const vec4* pCopy) {
  pSet->x = pCopy->x;
  pSet->y = pCopy->y;
  pSet->z = pCopy->z;
  pSet->w = pCopy->w;

  return pSet;  
};

// Returns the dot product between two vectors. 
// vec4 vecA, vecB;
// vec4Set(&vecA, 0.0, 0.1, 0.0, 1.0);
// vec4Set(&vecB, 0.1, 0.0, 1.0, 0.0);
// MATH3D_FLOAT dot = vec4Dot(&vecA, &vecB);
// dot is now 0.0
// Note that when the two vectors are unit vectors (normalized) vec4Dot actually returns the cosine value of the angle between the two vectors. Amazingly handy.
MATH3D_FLOAT vec4Dot(const vec4* pVecA, const vec4* pVecB) {
  return (pVecA->x * pVecB->x) + (pVecA->y * pVecB->y) + (pVecA->z * pVecB->z) + (pVecA->w * pVecB->w);
};

// Normalises our vector, this results in our vector becoming unit lenght.
// vec4 vector;
// vec4Set(&vector, 5.0, 0.0, 0.0, 0.0);
// vec4Normalise(&vector);
// vector is now (1.0, 0.0, 0.0, 0.0)
vec4* vec4Normalise(vec4 *pNormalise) {
  MATH3D_FLOAT dot = vec4Dot(pNormalise, pNormalise);
  if (dot <= MATH3D_EPSILON) {
    // can't normalize a vector with 0.0 length  
    pNormalise->x = 1.0;
    pNormalise->y = 0.0;
    pNormalise->z = 0.0;
    pNormalise->w = 0.0;
  } else {
    dot = sqrt(dot);
    pNormalise->x /= dot;
    pNormalise->y /= dot;
    pNormalise->z /= dot;
    pNormalise->w /= dot;
  };
  
  return pNormalise;
};

// Returns the lenght of a vector
// vec4 vector;
// vec4Set(&vector, 0.0, 5.0, 0.0, 0.0);
// MATH3D_FLOAT L = vec4Length(&vector);
// L is now 5.0
MATH3D_FLOAT vec4Lenght(const vec4* pLengthOf) {
  return sqrt(vec4Dot(pLengthOf, pLengthOf));
};

// Adds a vector to another vector:
// vec4 vecA, vecB;
// vec4Set(&vecA, 0.0, 0.1, 0.0, 0.0);
// vec4Set(&vecB, 0.1, 0.0, 0.0, 0.0);
// vec4Add(&vecA, &vecB);
// vecA is now (1.0, 1.0, 0.0, 0.0)
vec4* vec4Add(vec4* pAddTo, const vec4* pAdd) {
  pAddTo->x += pAdd->x;
  pAddTo->y += pAdd->y;
  pAddTo->z += pAdd->z;
  pAddTo->w += pAdd->w;
  
  return pAddTo;
};

// Subtracts a vector from another vector
// vec4 vecA, vecB;
// vec4Set(&vecA, 0.0, 0.1, 0.0, 0.0);
// vec4Set(&vecB, 0.1, 0.0, 0.0, 0.0);
// vec4Sub(&vecA, &vecB);
// vecA is now (-1.0, 1.0, 0.0, 0.0)
vec4* vec4Sub(vec4* pSubFrom, const vec4* pSub) {
  pSubFrom->x -= pSub->x;
  pSubFrom->y -= pSub->y;
  pSubFrom->z -= pSub->z;
  pSubFrom->w -= pSub->w;
  
  return pSubFrom;
};

// Multiply a vector with a constante
vec4* vec4Mult(vec4* pVec, const float pMult) {
  pVec->x = pVec->x * pMult;
  pVec->y = pVec->y * pMult;
  pVec->z = pVec->z * pMult;
  pVec->w = pVec->w * pMult;
  
  return pVec;
};

// Divide a vector by a constante
vec4* vec4Div(vec4* pVec, const float pDiv) {
  if (pDiv != 0.0) {
    pVec->x = pVec->x / pDiv;
    pVec->y = pVec->y / pDiv;
    pVec->z = pVec->z / pDiv;    
    pVec->w = pVec->w / pDiv;    
  };
  
  return pVec;  
};

// Scale a vector
// vec4 vec;
// vec4Set(&vec, 1.0, 2.0, 3.0, 4.0);
// vec4Scale(&vec, 2.0);
// vec is now (2.0, 4.0, 6.0, 8.0)
vec4* vec4Scale(vec4* pSet, const float pScale) {
  pSet->x *= pScale;
  pSet->y *= pScale;
  pSet->z *= pScale;
  pSet->w *= pScale;
  
  return pSet;  
};

// sets the xyz part of a vec4 to a vec3
vec3* vec3FromVec4(vec3* pSet, const vec4* pFrom, bool pApplyW) {
  pSet->x = pFrom->x;
  pSet->y = pFrom->y;
  pSet->z = pFrom->z;
  
  if (pApplyW && (pFrom->w != 0.0)) {
    pSet->x /= pFrom->w;
    pSet->y /= pFrom->w;
    pSet->z /= pFrom->w;
  };
  
  return pSet;
};

// copies a vec3 into a vec4 using the specified W value
vec4* vec4FromVec3(vec4* pSet, const vec3* pFrom, MATH3D_FLOAT pW) {
  pSet->x = pFrom->x;
  pSet->y = pFrom->y;
  pSet->z = pFrom->z;
  pSet->w = pW;
  
  return pSet;
};

////////////////////////////////////////////////////////////////////////////////////
// mat3

// Copies the contents of one 3x3 matrix into another
mat3* mat3Copy(mat3* pDest, const mat3* pSource) {
  int i,j;
  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      pDest->m[j][i] = pSource->m[j][i];
    };    
  };
  
  return pDest;  
};

// Initialises a 3x3 matrix as an identity matrix i.e.:
// 1.0, 0.0, 0.0
// 0.0, 1.0, 0.0
// 0.0, 0.0, 1.0
mat3* mat3Identity(mat3* pSet) {
  int i,j;
  for (j = 0 ; j < 3; j++) {
    for (i = 0 ; i < 3; i++) {
      pSet->m[j][i] = i == j ? 1 : 0;
    };    
  };
  
  return pSet;
};

// Transpose flips rows and columns around
mat3* mat3Transpose(mat3* pTranspose) {
  int i,j;
  mat3 copy;
  mat3Copy(&copy, pTranspose);
  
  for (j = 0 ; j < 3; j++) {
    for (i = 0 ; i < 3; i++) {
      pTranspose->m[j][i] = copy.m[i][j];
    };    
  };
  
  return pTranspose;  
};

// Applies a matrix to a vector, it is safe to use the same variable for pSet and pApplyTo
vec3* mat3ApplyToVec3(vec3* pSet, const vec3* pApplyTo, const mat3* pMatrix) {
  vec3 applyTo;
  
  // make a copy to apply to so pSet can equal pApplyTo
  vec3Copy(&applyTo, pApplyTo);
  
  pSet->x = (applyTo.x * pMatrix->m[0][0]) + (applyTo.y * pMatrix->m[1][0]) + (applyTo.z * pMatrix->m[2][0]);
  pSet->y = (applyTo.x * pMatrix->m[0][1]) + (applyTo.y * pMatrix->m[1][1]) + (applyTo.z * pMatrix->m[2][1]);
  pSet->z = (applyTo.x * pMatrix->m[0][2]) + (applyTo.y * pMatrix->m[1][2]) + (applyTo.z * pMatrix->m[2][2]);
  
  return pSet;
};

// Multiplies two matrices together. 
// If you would apply a series of matrices one by one to a vertices like this:
// mat3ApplyToVec3(&vector, &vector, &matrixA);
// mat3ApplyToVec3(&vector, &vector, &matrixB);
// mat3ApplyToVec3(&vector, &vector, &matrixC);
// Would be the same as:
// mat3Identity(&matrix);
// mat3Multiply(&matrix, &matrixC);
// mat3Multiply(&matrix, &matrixB);
// mat3Multiply(&matrix, &matrixA);
// mat3ApplyToVec3(&vector, &vector, &matrix);
// This allows you to calculate a single matrix for a series of transformations and then apply it to a list of vertices. 
mat3* mat3Multiply(mat3* pMultTo, const mat3* pMultWith) {
  int   i, j, k;
  mat3  Copy;

  mat3Copy(&Copy, pMultTo);

  for (j = 0; j < 3; j++) {
    for (i = 0; i < 3; i++) {
      pMultTo->m[j][i] = 0.0;

      for (k = 0; k < 3; k++) {
        pMultTo->m[j][i] += pMultWith->m[j][k] * Copy.m[k][i];        
      };
    };
  };
  
  return pMultTo;
};

// Applies a rotation matrix to our matrix by rotating around an axis by a certain degrees
// mat3 rotate;
// vec3 axis;
// mat3Identity(&rotate);
// vec3Set(&axis, 1.0, 1.0, 1.0);
// mat3Rotate(&rotate, 45.0, &axis);
// ... now you can apply this rotation matrix to your vertices ...
mat3* mat3Rotate(mat3* pMatrix, MATH3D_FLOAT pAngle, const vec3* pAxis) {
  mat3 R;
  vec3 axis;
  MATH3D_FLOAT sinus, cosine, xx, yy, zz, xy, yz, zx, xs, ys, zs, oneMinCos;
  
  vec3Normalise(vec3Copy(&axis, pAxis));

  sinus = sinf(angToRad(pAngle));
  cosine = cosf(angToRad(pAngle));
  
  xx = axis.x * axis.x;
  yy = axis.y * axis.y;
  zz = axis.z * axis.z;
  xy = axis.x * axis.y;
  yz = axis.y * axis.z;
  zx = axis.z * axis.x;
  xs = axis.x * sinus;
  ys = axis.y * sinus;
  zs = axis.z * sinus;
  oneMinCos = 1.0f - cosine;
  
  R.m[0][0] = (oneMinCos * xx) + cosine;
  R.m[0][1] = (oneMinCos * xy) - zs;
  R.m[0][2] = (oneMinCos * zx) + ys;
 
  R.m[1][0] = (oneMinCos * xy) + zs;
  R.m[1][1] = (oneMinCos * yy) + cosine;
  R.m[1][2] = (oneMinCos * yz) - xs;
 
  R.m[2][0] = (oneMinCos * zx) - ys;
  R.m[2][1] = (oneMinCos * yz) + xs;
  R.m[2][2] = (oneMinCos * zz) + cosine;

  return mat3Multiply(pMatrix, &R);
};

// Apply a scale to our matrix
mat3* mat3Scale(mat3 *pMatrix, const vec3* pScale) {
  mat3 S;
  
  mat3Identity(&S);
  S.m[0][0] = pScale->x;
  S.m[1][1] = pScale->y;
  S.m[2][2] = pScale->z;
  
  return mat3Multiply(pMatrix, &S);
};

////////////////////////////////////////////////////////////////////////////////////
// mat4

// Copies the contents of one 4x4 matrix into another
mat4* mat4Copy(mat4* pDest, const mat4* pSource) {
  int i,j;
  for (j = 0 ; j < 4; j++) {
    for (i = 0 ; i < 4; i++) {
      pDest->m[j][i] = pSource->m[j][i];
    };    
  };
  
  return pDest;  
};

// Initialises a 4x4 matrix as an identity matrix i.e.:
// 1.0, 0.0, 0.0, 0.0
// 0.0, 1.0, 0.0, 0.0
// 0.0, 0.0, 1.0, 0.0
// 0.0, 0.0, 0.0, 1.0
mat4* mat4Identity(mat4* pSet) {
  int i,j;
  for (j = 0 ; j < 4; j++) {
    for (i = 0 ; i < 4; i++) {
      pSet->m[j][i] = i == j ? 1 : 0;
    };    
  };
  
  return pSet;
};

// Transpose flips rows and columns around
mat4* mat4Transpose(mat4* pTranspose) {
  int  i,j;
  mat4 copy;
  mat4Copy(&copy, pTranspose);
  
  for (j = 0 ; j < 4; j++) {
    for (i = 0 ; i < 4; i++) {
      pTranspose->m[j][i] = copy.m[i][j];
    };    
  };
  
  return pTranspose;  
};

// Applies a matrix to a vector, it is safe to use the same variable for pSet and pApplyTo
vec3* mat4ApplyToVec3(vec3* pSet, const vec3* pApplyTo, const mat4* pMatrix) {
  vec4 vecA, vecB;

  // turn it into a vec4 before we can apply our matrix
  vec4FromVec3(&vecA, pApplyTo, 1.0);
  mat4ApplyToVec4(&vecB, &vecA, pMatrix);

  // now get our vec3 after dividing by w
  vec3FromVec4(pSet, &vecB, true);
  return pSet;
};

// Applies a matrix to a vector, it is safe to use the same variable for pSet and pApplyTo
vec4* mat4ApplyToVec4(vec4* pSet, const vec4* pApplyTo, const mat4* pMatrix) {
  vec4 applyTo;
  
  // make a copy to apply to so pSet can equal pApplyTo
  vec4Copy(&applyTo, pApplyTo);
  
  pSet->x = (applyTo.x * pMatrix->m[0][0]) + (applyTo.y * pMatrix->m[1][0]) + (applyTo.z * pMatrix->m[2][0]) + (applyTo.w * pMatrix->m[3][0]);
  pSet->y = (applyTo.x * pMatrix->m[0][1]) + (applyTo.y * pMatrix->m[1][1]) + (applyTo.z * pMatrix->m[2][1]) + (applyTo.w * pMatrix->m[3][1]);
  pSet->z = (applyTo.x * pMatrix->m[0][2]) + (applyTo.y * pMatrix->m[1][2]) + (applyTo.z * pMatrix->m[2][2]) + (applyTo.w * pMatrix->m[3][2]);
  pSet->w = (applyTo.x * pMatrix->m[0][3]) + (applyTo.y * pMatrix->m[1][3]) + (applyTo.z * pMatrix->m[2][3]) + (applyTo.w * pMatrix->m[3][3]);
  
  return pSet;
};

// Multiplies two matrices together. 
// see mat4Multiply
mat4* mat4Multiply(mat4* pMultTo, const mat4* pMultWith) {
  int    i, j, k;
  mat4  Copy;

  mat4Copy(&Copy, pMultTo);

  for (j = 0; j < 4; j++) {
    for (i = 0; i < 4; i++) {
      pMultTo->m[j][i] = 0.0;

      for (k = 0; k < 4; k++) {
        pMultTo->m[j][i] += pMultWith->m[j][k] * Copy.m[k][i];        
      };
    };
  };
  
  return pMultTo;
};

// Set the inverse of a 4x4 matrix
mat4* mat4Inverse(mat4* pInverse, const mat4* pMatrix) {
  // Based on http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
  
  MATH3D_FLOAT det;
  int i,j;

  pInverse->m[0][0] =  pMatrix->m[1][1] * pMatrix->m[2][2] * pMatrix->m[3][3] - 
                       pMatrix->m[1][1] * pMatrix->m[2][3] * pMatrix->m[3][2] - 
                       pMatrix->m[2][1] * pMatrix->m[1][2] * pMatrix->m[3][3] + 
                       pMatrix->m[2][1] * pMatrix->m[1][3] * pMatrix->m[3][2] +
                       pMatrix->m[3][1] * pMatrix->m[1][2] * pMatrix->m[2][3] - 
                       pMatrix->m[3][1] * pMatrix->m[1][3] * pMatrix->m[2][2];

  pInverse->m[1][0] = -pMatrix->m[1][0] * pMatrix->m[2][2] * pMatrix->m[3][3] + 
                       pMatrix->m[1][0] * pMatrix->m[2][3] * pMatrix->m[3][2] + 
                       pMatrix->m[2][0] * pMatrix->m[1][2] * pMatrix->m[3][3] - 
                       pMatrix->m[2][0] * pMatrix->m[1][3] * pMatrix->m[3][2] - 
                       pMatrix->m[3][0] * pMatrix->m[1][2] * pMatrix->m[2][3] + 
                       pMatrix->m[3][0] * pMatrix->m[1][3] * pMatrix->m[2][2];

  pInverse->m[2][0] =  pMatrix->m[1][0] * pMatrix->m[2][1] * pMatrix->m[3][3] - 
                       pMatrix->m[1][0] * pMatrix->m[2][3] * pMatrix->m[3][1] - 
                       pMatrix->m[2][0] * pMatrix->m[1][1] * pMatrix->m[3][3] + 
                       pMatrix->m[2][0] * pMatrix->m[1][3] * pMatrix->m[3][1] + 
                       pMatrix->m[3][0] * pMatrix->m[1][1] * pMatrix->m[2][3] - 
                       pMatrix->m[3][0] * pMatrix->m[1][3] * pMatrix->m[2][1];

  pInverse->m[3][0] = -pMatrix->m[1][0] * pMatrix->m[2][1] * pMatrix->m[3][2] + 
                       pMatrix->m[1][0] * pMatrix->m[2][2] * pMatrix->m[3][1] +
                       pMatrix->m[2][0] * pMatrix->m[1][1] * pMatrix->m[3][2] - 
                       pMatrix->m[2][0] * pMatrix->m[1][2] * pMatrix->m[3][1] - 
                       pMatrix->m[3][0] * pMatrix->m[1][1] * pMatrix->m[2][2] + 
                       pMatrix->m[3][0] * pMatrix->m[1][2] * pMatrix->m[2][1];

  pInverse->m[0][1] = -pMatrix->m[0][1] * pMatrix->m[2][2] * pMatrix->m[3][3] + 
                       pMatrix->m[0][1] * pMatrix->m[2][3] * pMatrix->m[3][2] + 
                       pMatrix->m[2][1] * pMatrix->m[0][2] * pMatrix->m[3][3] - 
                       pMatrix->m[2][1] * pMatrix->m[0][3] * pMatrix->m[3][2] - 
                       pMatrix->m[3][1] * pMatrix->m[0][2] * pMatrix->m[2][3] + 
                       pMatrix->m[3][1] * pMatrix->m[0][3] * pMatrix->m[2][2];

  pInverse->m[1][1] =  pMatrix->m[0][0] * pMatrix->m[2][2] * pMatrix->m[3][3] - 
                       pMatrix->m[0][0] * pMatrix->m[2][3] * pMatrix->m[3][2] - 
                       pMatrix->m[2][0] * pMatrix->m[0][2] * pMatrix->m[3][3] + 
                       pMatrix->m[2][0] * pMatrix->m[0][3] * pMatrix->m[3][2] + 
                       pMatrix->m[3][0] * pMatrix->m[0][2] * pMatrix->m[2][3] - 
                       pMatrix->m[3][0] * pMatrix->m[0][3] * pMatrix->m[2][2];

  pInverse->m[2][1] = -pMatrix->m[0][0] * pMatrix->m[2][1] * pMatrix->m[3][3] + 
                       pMatrix->m[0][0] * pMatrix->m[2][3] * pMatrix->m[3][1] + 
                       pMatrix->m[2][0] * pMatrix->m[0][1] * pMatrix->m[3][3] - 
                       pMatrix->m[2][0] * pMatrix->m[0][3] * pMatrix->m[3][1] - 
                       pMatrix->m[3][0] * pMatrix->m[0][1] * pMatrix->m[2][3] + 
                       pMatrix->m[3][0] * pMatrix->m[0][3] * pMatrix->m[2][1];

  pInverse->m[3][1] =  pMatrix->m[0][0] * pMatrix->m[2][1] * pMatrix->m[3][2] - 
                       pMatrix->m[0][0] * pMatrix->m[2][2] * pMatrix->m[3][1] - 
                       pMatrix->m[2][0] * pMatrix->m[0][1] * pMatrix->m[3][2] + 
                       pMatrix->m[2][0] * pMatrix->m[0][2] * pMatrix->m[3][1] + 
                       pMatrix->m[3][0] * pMatrix->m[0][1] * pMatrix->m[2][2] - 
                       pMatrix->m[3][0] * pMatrix->m[0][2] * pMatrix->m[2][1];

  pInverse->m[0][2] =  pMatrix->m[0][1] * pMatrix->m[1][2] * pMatrix->m[3][3] - 
                       pMatrix->m[0][1] * pMatrix->m[1][3] * pMatrix->m[3][2] - 
                       pMatrix->m[1][1] * pMatrix->m[0][2] * pMatrix->m[3][3] + 
                       pMatrix->m[1][1] * pMatrix->m[0][3] * pMatrix->m[3][2] + 
                       pMatrix->m[3][1] * pMatrix->m[0][2] * pMatrix->m[1][3] - 
                       pMatrix->m[3][1] * pMatrix->m[0][3] * pMatrix->m[1][2];

  pInverse->m[1][2] = -pMatrix->m[0][0] * pMatrix->m[1][2] * pMatrix->m[3][3] + 
                       pMatrix->m[0][0] * pMatrix->m[1][3] * pMatrix->m[3][2] + 
                       pMatrix->m[1][0] * pMatrix->m[0][2] * pMatrix->m[3][3] - 
                       pMatrix->m[1][0] * pMatrix->m[0][3] * pMatrix->m[3][2] - 
                       pMatrix->m[3][0] * pMatrix->m[0][2] * pMatrix->m[1][3] + 
                       pMatrix->m[3][0] * pMatrix->m[0][3] * pMatrix->m[1][2];

  pInverse->m[2][2] =  pMatrix->m[0][0] * pMatrix->m[1][1] * pMatrix->m[3][3] - 
                       pMatrix->m[0][0] * pMatrix->m[1][3] * pMatrix->m[3][1] - 
                       pMatrix->m[1][0] * pMatrix->m[0][1] * pMatrix->m[3][3] + 
                       pMatrix->m[1][0] * pMatrix->m[0][3] * pMatrix->m[3][1] + 
                       pMatrix->m[3][0] * pMatrix->m[0][1] * pMatrix->m[1][3] - 
                       pMatrix->m[3][0] * pMatrix->m[0][3] * pMatrix->m[1][1];

  pInverse->m[3][2] = -pMatrix->m[0][0] * pMatrix->m[1][1] * pMatrix->m[3][2] + 
                       pMatrix->m[0][0] * pMatrix->m[1][2] * pMatrix->m[3][1] + 
                       pMatrix->m[1][0] * pMatrix->m[0][1] * pMatrix->m[3][2] - 
                       pMatrix->m[1][0] * pMatrix->m[0][2] * pMatrix->m[3][1] - 
                       pMatrix->m[3][0] * pMatrix->m[0][1] * pMatrix->m[1][2] + 
                       pMatrix->m[3][0] * pMatrix->m[0][2] * pMatrix->m[1][1];

  pInverse->m[0][3] = -pMatrix->m[0][1] * pMatrix->m[1][2] * pMatrix->m[2][3] + 
                       pMatrix->m[0][1] * pMatrix->m[1][3] * pMatrix->m[2][2] + 
                       pMatrix->m[1][1] * pMatrix->m[0][2] * pMatrix->m[2][3] - 
                       pMatrix->m[1][1] * pMatrix->m[0][3] * pMatrix->m[2][2] - 
                       pMatrix->m[2][1] * pMatrix->m[0][2] * pMatrix->m[1][3] + 
                       pMatrix->m[2][1] * pMatrix->m[0][3] * pMatrix->m[1][2];

  pInverse->m[1][3] =  pMatrix->m[0][0] * pMatrix->m[1][2] * pMatrix->m[2][3] - 
                       pMatrix->m[0][0] * pMatrix->m[1][3] * pMatrix->m[2][2] - 
                       pMatrix->m[1][0] * pMatrix->m[0][2] * pMatrix->m[2][3] + 
                       pMatrix->m[1][0] * pMatrix->m[0][3] * pMatrix->m[2][2] + 
                       pMatrix->m[2][0] * pMatrix->m[0][2] * pMatrix->m[1][3] - 
                       pMatrix->m[2][0] * pMatrix->m[0][3] * pMatrix->m[1][2];

  pInverse->m[2][3] = -pMatrix->m[0][0] * pMatrix->m[1][1] * pMatrix->m[2][3] + 
                       pMatrix->m[0][0] * pMatrix->m[1][3] * pMatrix->m[2][1] + 
                       pMatrix->m[1][0] * pMatrix->m[0][1] * pMatrix->m[2][3] - 
                       pMatrix->m[1][0] * pMatrix->m[0][3] * pMatrix->m[2][1] - 
                       pMatrix->m[2][0] * pMatrix->m[0][1] * pMatrix->m[1][3] + 
                       pMatrix->m[2][0] * pMatrix->m[0][3] * pMatrix->m[1][1];

  pInverse->m[3][3] =  pMatrix->m[0][0] * pMatrix->m[1][1] * pMatrix->m[2][2] - 
                       pMatrix->m[0][0] * pMatrix->m[1][2] * pMatrix->m[2][1] - 
                       pMatrix->m[1][0] * pMatrix->m[0][1] * pMatrix->m[2][2] + 
                       pMatrix->m[1][0] * pMatrix->m[0][2] * pMatrix->m[2][1] + 
                       pMatrix->m[2][0] * pMatrix->m[0][1] * pMatrix->m[1][2] - 
                       pMatrix->m[2][0] * pMatrix->m[0][2] * pMatrix->m[1][1];
  
  det = pMatrix->m[0][0] * pInverse->m[0][0] + 
        pMatrix->m[0][1] * pInverse->m[1][0] + 
        pMatrix->m[0][2] * pInverse->m[2][0] + 
        pMatrix->m[0][3] * pInverse->m[3][0];

  if (det == 0) {
    return NULL;    
  };

  det = 1.0 / det;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      pInverse->m[i][j] = pInverse->m[i][j] * det;
    };
  };
       
  return pInverse;
};

// Applies a rotation matrix to our matrix by rotating around an axis by a certain degrees
// See mat3Rotate
mat4* mat4Rotate(mat4* pMatrix, MATH3D_FLOAT pAngle, const vec3* pAxis) {
  mat3 rotate3;
  mat4 rotate4;
  
  mat3Identity(&rotate3);
  mat3Rotate(&rotate3, pAngle, pAxis);
  
  mat4FromMat3(&rotate4, &rotate3);
  
  return mat4Multiply(pMatrix, &rotate4);
};

// Applies a translation matrix to our matrix (move)
mat4* mat4Translate(mat4 *pMatrix, const vec3* pAxis) {
  mat4 translate;
  
  mat4Identity(&translate);
  translate.m[3][0] = pAxis->x;
  translate.m[3][1] = pAxis->y;
  translate.m[3][2] = pAxis->z;

  return mat4Multiply(pMatrix, &translate);
};

// Apply a scale to our matrix
mat4* mat4Scale(mat4 *pMatrix, const vec3* pScale) {
  mat4 S;
  
  mat4Identity(&S);
  S.m[0][0] = pScale->x;
  S.m[1][1] = pScale->y;
  S.m[2][2] = pScale->z;
  
  return mat4Multiply(pMatrix, &S);
};

// Applies an orthographic projection. This allows a 2D projection where Z is only used for layering.
// Note that top and bottom are swapped as in OpenGL it is custom to have 0,0 be at the left bottom and width,height the top right.
// mat4 projection;
// mat4Identity(&projection);
// mat4Ortho(&projection, 0, screenwidth, 0, screenheight, 0.1, 100.0);
mat4* mat4Ortho(mat4* pMatrix, MATH3D_FLOAT pLeft, MATH3D_FLOAT pRight, MATH3D_FLOAT pBottom, MATH3D_FLOAT pTop, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar) {
  mat4 M;

  M.m[0][0] = 2.0f / (pRight - pLeft);
  M.m[0][1] = 0.0f;
  M.m[0][2] = 0.0f;
  M.m[0][3] = 0.0f;

  M.m[1][0] = 0.0f;
  M.m[1][1] = 2.0f / (pTop - pBottom);
  M.m[1][2] = 0.0f;
  M.m[1][3] = 0.0f;
  
  M.m[2][0] = 0.0f;
  M.m[2][1] = 0.0f;
  M.m[2][2] = -2.0f / (pZFar - pZNear);
  M.m[2][3] = 0.0f;

  M.m[3][0] = -(pRight + pLeft) / (pRight - pLeft);
  M.m[3][1] = -(pTop + pBottom) / (pTop - pBottom);
  M.m[3][2] = -(pZFar + pZNear) / (pZFar - pZNear);
  M.m[3][3] = 1.0f;
  
  return mat4Multiply(pMatrix, &M);  
};

// Applies a matrix based on a frustum for 3D projection.
mat4* mat4Frustum(mat4* pMatrix, MATH3D_FLOAT pLeft, MATH3D_FLOAT pRight, MATH3D_FLOAT pBottom, MATH3D_FLOAT pTop, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar) {
  mat4 M;

  M.m[0][0] = (2.0f * pZNear) / (pRight - pLeft);
  M.m[0][1] = 0.0;
  M.m[0][2] = 0.0;
  M.m[0][3] = 0.0f;

  M.m[1][0] = 0.0;
  M.m[1][1] = (2 * pZNear) / (pTop - pBottom);
  M.m[1][2] = 0.0;
  M.m[1][3] = 0.0f;
  
  M.m[2][0] = (pRight + pLeft) / (pRight - pLeft);
  M.m[2][1] = (pTop + pBottom) / (pTop - pBottom);
  M.m[2][2] = -(pZFar + pZNear) / (pZFar - pZNear);
  M.m[2][3] = -1.0f;

  M.m[3][0] = 0.0;
  M.m[3][1] = 0.0;
  M.m[3][2] = -(2.0f * pZFar * pZNear) / (pZFar - pZNear);
  M.m[3][3] = 0.0f;  
  
  return mat4Multiply(pMatrix, &M);  
};

// Applies a 3D projection matrix.
// pMatrix = pointer to matrix to populate, should contain an identity matrix
// pFOV = Field of View
// pAspect = Aspect ratio of the screen
// pZNear = Near distance
// pZFar = Far distance
mat4* mat4Projection(mat4* pMatrix, MATH3D_FLOAT pFOV, MATH3D_FLOAT pAspect, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar) {
  MATH3D_FLOAT ymax, xmax;
  
  ymax = pZNear * tan(pFOV * PI / 360.0f);
  xmax = ymax * pAspect;
  
  return mat4Frustum(pMatrix, -xmax, xmax, -ymax, ymax, pZNear, pZFar);
};

// Applies a 3D projection matrix for stereo scopic rendering
// Same parameters as mat4Projection but with additional:
// - pIOD = intraocular distance, distance between the two eyes (on average 6.5cm)
// - pProjPlane = projection plane, distance from eye at which our frustrums intersect (also known as convergence)
// - pMode = 0 => center eye, 1 => left eye, 2 => right eye (so 0 results in same projection as mat4Projection)
mat4* mat4Stereo(mat4* pMatrix, MATH3D_FLOAT pFOV, MATH3D_FLOAT pAspect, MATH3D_FLOAT pZNear, MATH3D_FLOAT pZFar, float pIOD, float pProjPlane, int pMode) {
  MATH3D_FLOAT left, right, modeltranslation, ymax, xmax, frustumshift;
  vec3 tmpVector;

  ymax = pZNear * tan(pFOV * PI / 360.0f);
  xmax = ymax * pAspect;
  frustumshift = (pIOD/2)*pZNear/pProjPlane;

  switch (pMode) {
    case 1: { // left eye
      left = -xmax + frustumshift;
      right = xmax + frustumshift;
      modeltranslation = pIOD / 2.0;
    }; break;
    case 2: { // right eye
      left = -xmax - frustumshift;
      right = xmax - frustumshift;
      modeltranslation = -pIOD / 2.0;
    }; break;
    default: {
      left = -xmax;
      right = xmax;
      modeltranslation = 0.0;
    }; break;
  };
  
  mat4Frustum(pMatrix, left, right, -ymax, ymax, pZNear, pZFar);
  mat4Translate(pMatrix, vec3Set(&tmpVector, modeltranslation, 0.0, 0.0));
  
  return pMatrix;
};

// Applies a look-at matrix to create a view matrix
// pEye is the position of our camera
// pLookat is the position we're looking at
// pUp is the vector that points up (make sure this is a normalised vector!)
mat4* mat4LookAt(mat4* pMatrix, const vec3* pEye, const vec3* pLookat, const vec3* pUp) {
  mat4            M;
  vec3            xaxis, yaxis, zaxis;
  MATH3D_FLOAT    dot;
  
  // zaxis = pEye - pLookat, the direction we are looking at
  vec3Copy(&zaxis, pEye);
  vec3Sub(&zaxis, pLookat);
  vec3Normalise(&zaxis);
  
  // check if we're looking straight up, if so this calculation fails
  dot = vec3Dot(&zaxis, pUp);
	if (dot==1.0) {
    // right is right, unless thats strangely enough up...
    vec3Set(&xaxis, 1.0, 0.0, 0.0);
	} else {
    // Cross vector gets the vector that is perpendicular to the plain formed by the other two vectors. So with up and our direction of looking, it gives our right vector
    vec3Cross(&xaxis, pUp, &zaxis);
	};
  vec3Normalise(&xaxis);
	
  // And we do the same for our real "up" vector which is now perpendicular to our right (X) and forward (Z) vectors
  vec3Cross(&yaxis, &zaxis, &xaxis);
  vec3Normalise(&yaxis);
	  
	M.m[0][0] = xaxis.x;
	M.m[0][1] = yaxis.x;
	M.m[0][2] = zaxis.x;
	M.m[0][3] = 0.0f;

	M.m[1][0] = xaxis.y;
	M.m[1][1] = yaxis.y;
	M.m[1][2] = zaxis.y;
	M.m[1][3] = 0.0f;
	
	M.m[2][0] = xaxis.z;
	M.m[2][1] = yaxis.z;
	M.m[2][2] = zaxis.z;
	M.m[2][3] = 0.0f;

	M.m[3][0] = 0.0;
	M.m[3][1] = 0.0;
	M.m[3][2] = 0.0;
	M.m[3][3] = 1.0f;
  
  mat4Multiply(pMatrix, &M);
  
  // now translate to our eye position
  mat4Identity(&M);
	M.m[3][0] = -pEye->x;
	M.m[3][1] = -pEye->y;
	M.m[3][2] = -pEye->z;
    
  return mat4Multiply(pMatrix, &M);  
};

// Initialize a 3x3 matrix from a 4x4 matrix, discards the last column and row
mat3* mat3FromMat4(mat3* pSet, const mat4* pFrom) {
  int i,j;
  for (j = 0 ; j < 3; j++) {
    for (i = 0 ; i < 3; i++) {
      pSet->m[j][i] = pFrom->m[j][i];
    };    
  };
  
  return pSet;
};

// Initialize a 4x4 matrix from a 3x3 matrix
mat4* mat4FromMat3(mat4* pSet, const mat3* pFrom) {
  int i,j;
  mat4Identity(pSet);
  
  for (j = 0 ; j < 3; j++) {
    for (i = 0 ; i < 3; i++) {
      pSet->m[j][i] = pFrom->m[j][i];
    };    
  };
  
  return pSet;
};


#endif /* MATH3D_IMPLEMENTATION */

#endif /* !math3dh */
