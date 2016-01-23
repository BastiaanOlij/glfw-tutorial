/********************************************************
 * dynamicarray.h - generic dynamic array 
 * by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define DYNARRAY_IMPLEMENTATION
 *
 * Revision history:
 * 0.1  16-01-2016  First version with basic functions
 *
 ********************************************************/

#ifndef dynarrayh
#define dynarrayh

#define DYNARRAY_NOENTRY      0xffffffff
#define DYNARRAY_NOMEM        0xfffffffe
#define DYNARRAY_EXPAND       100

// We need bool, it seems to be define differently over platforms
#ifndef bool
typedef int bool;
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};
#endif 

typedef struct dynarray {
  unsigned int        entrySize;      // size of a single entry
  unsigned int        numEntries;     // number of entries contained in our array
  unsigned int        maxEntries;     // number of entries that fit into our current array
  void *              data;           // pointer to our data buffer
} dynarray;

#ifdef __cplusplus
extern "C" {
#endif
  
dynarray * newDynArray(unsigned int pEntrySize);
void dynArrayFree(dynarray * pArray);
bool dynArrayCheckSize(dynarray * pArray, unsigned int pMinSize);
unsigned int dynArrayPush(dynarray * pArray, void * pData);
void * dynArrayDataAtIndex(dynarray * pArray, unsigned int pIndex);
  
#ifdef __cplusplus
};
#endif 

#ifdef DYNARRAY_IMPLEMENTATION

// creates a new dynamic list
// dynarray * myArray = newDynArray(sizeof(mystruct));
dynarray * newDynArray(unsigned int pEntrySize) {
  dynarray * newArray = (dynarray *) malloc(sizeof(dynarray));
  if (newArray != NULL) {
    newArray->entrySize = pEntrySize == 0 ? 1 : pEntrySize; // we don't do 0...
    newArray->numEntries = 0;
    newArray->maxEntries = 0;
    newArray->data = NULL;
  };
  return newArray;
};

// frees the space associated with the array, does not do anything with any pointers to allocated memory inside of the data
// dynArrayFree(myArray);
void dynArrayFree(dynarray * pArray) {
  if (pArray == NULL) {
    return;
  };
  
  if (pArray->data != NULL) {
    free(pArray->data);
    pArray->numEntries = 0;
    pArray->maxEntries = 0;
    pArray->data = NULL;
  };
  
  // and free our array
  free(pArray);
};

// check if we have enough space in our array
// dynArrayCheckSize(myArray, 15); // make sure we can hold 15 entries before realloc needs to be called
bool dynArrayCheckSize(dynarray * pArray, unsigned int pMinSize) {
  if (pArray == NULL) {
    return false;
  };
  
  if (pArray->data == NULL) {
    pArray->maxEntries = pMinSize > DYNARRAY_EXPAND ? pMinSize : DYNARRAY_EXPAND;
    pArray->data = malloc(pArray->entrySize * pArray->maxEntries);
  } else if (pArray->maxEntries < pMinSize) {
    pArray->maxEntries += DYNARRAY_EXPAND;
    if (pArray->maxEntries < pMinSize) {
      // still not large enough? weird...
      pArray->maxEntries = pMinSize;
    };
    pArray->data = realloc(pArray->data, pArray->entrySize * pArray->maxEntries);
  };

  if (pArray->data == NULL) {
    // something must have gone wrong!!
    pArray->numEntries = 0;
    pArray->maxEntries = 0;
  };
  
  return pArray->data != NULL;
};

// adds an entry at the end of our array
// dynArrayPush(myArray, &myData);
unsigned int dynArrayPush(dynarray * pArray, void * pData) {
  if (pArray == NULL) {
    return DYNARRAY_NOENTRY;
  } else if (!dynArrayCheckSize(pArray, pArray->numEntries + 1)) {
    return DYNARRAY_NOMEM;
  };

  // copy our data in place
  memcpy((char *) pArray->data + (pArray->numEntries * pArray->entrySize), pData, pArray->entrySize);

  // return and advance
  return pArray->numEntries++;
};

// returns a pointer to the entry at that location, it returns NULL if pIndex is out of bounds
// as this is a pointer to the entry inside of our array you can both retrieve and change the data within our array
// mystruct * data = dynArrayDataAtIndex(myArray, 15);
void * dynArrayDataAtIndex(dynarray * pArray, unsigned int pIndex) {
  if (pArray == NULL) {
    return NULL;
  } else if (pIndex >= pArray->numEntries) {
    // out of bounds
    return NULL;
  };
  
  return (char *) pArray->data + (pIndex * pArray->entrySize);
};

#endif /* DYNARRAY_IMPLEMENTATION */

#endif /* !dynarrayh */
