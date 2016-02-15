	/********************************************************
 * varchar.h - variable length char by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define VARCHAR_IMPLEMENTATION
 *
 * Revision history:
 * 0.1  15-01-2016  First version with basic functions
 *
 ********************************************************/

#ifndef varcharh
#define varcharh

// include support libraries
#include <stdbool.h>

#define VARCHAR_INCREASE  100
#define VARCHAR_NOTFOUND  0xffffffff

typedef struct varchar {
  char *        text;                 // our text buffer
  unsigned int  retainCount;          // retain count for this object
  unsigned int  size;                 // size of our buffer
  unsigned int  len;                  // length of our text string
} varchar;

#ifdef __cplusplus
extern "C" {
#endif

varchar * newVarchar();
void varcharRetain(varchar * pVarchar);
void varcharRelease(varchar * pVarchar);
int varcharCmp(varchar * pVarchar, const char *pWith);
unsigned int varcharPos(varchar * pVarchar, const char * pFind, unsigned int pSearchFrom);
varchar * varcharMid(varchar * pVarchar, unsigned int pStart, unsigned int pLen);
bool varcharAppend(varchar * pVarchar, const char * pText, unsigned int pLen);
void varcharTrim(varchar * pVarchar);

#ifdef __cplusplus
};
#endif 

#ifdef VARCHAR_IMPLEMENTATION

// checks if we need to enlarge our allocated buffer always allocating an extra byte for our zero terminater.
bool checkSize(varchar * pVarchar, unsigned int pSize) {
  if (pVarchar == NULL) {
    return false;
  } else if (pVarchar->size >= pSize) {
    // we have enough space
    return true;
  };
  
  // allocate memory
  if (pVarchar->text == NULL) {
    pVarchar->text = (char *) malloc(pSize + 1);
  } else {
    pVarchar->text = (char *) realloc(pVarchar->text, pSize + 1);
  };
  
  // did we succeed?
  if (pVarchar->text == NULL) {
    pVarchar->size = 0;
    pVarchar->len = 0;
    return false;
  } else {
    pVarchar->size = pSize;
    pVarchar->text[pVarchar->len] = '\0'; // just in case
  };
  
  return true;
};

// create a new varchar object
// varchar * myVarchar = newVarchar()
varchar * newVarchar() {
  varchar * newChar = (varchar *) malloc(sizeof(varchar));
  if (newChar != NULL) {
    newChar->retainCount = 1;
    newChar->text = NULL;
    newChar->size = 0;
    newChar->len = 0;
  };
  
  return newChar;
};

// increase our retain count, call this when you wish to hang on to a pointer to this object
// void setName(someobject * pThis, varchar * pName) {
//   pThis->myName = pVarchar;
//   if (pThis->myName != NULL) {}
//     varcharRetain(pThis->myName);
//   };
//   ...
void varcharRetain(varchar * pVarchar) {
  if (pVarchar != NULL) {
    pVarchar->retainCount++;
  };
};

// decreases the retain count, once it reaches zero the object is destroyed
// call this when you no longer need your copy of the pointer
// if (myVarchar != NULL) {
//   varcharRelease(myVarchar);
//   myVarchar = NULL; // we can no longer trust our pointer so set to NULL
// };
//
// Often used in combination with varcharRetain like this:
// void setName(someobject * pThis, varchar * pName) {
//   // nothing to do if its the same
//   if (pThis->myName == pName) {
//      return;
//   };
//
//   // out with the old...
//   if (pThis->myName != NULL) {
//     varcharRelease(pThis->myName);
//     pThis->myName = NULL; // we can no longer trust our pointer so set to NULL
//   };
//
//   // in with the new
//   pThis->myName = pVarchar;
//   if (pThis->myName != NULL) {}
//     varcharRetain(pThis->myName);
//   };
// };
void varcharRelease(varchar * pVarchar) {
  if (pVarchar == NULL) {
    return;
  } else if (pVarchar->retainCount > 1) {
    pVarchar->retainCount--;
    
    return;
  };
  
  // free our text buffer
  if (pVarchar->text != NULL) {
    free(pVarchar->text);
    pVarchar->text = NULL;
    pVarchar->size = 0;
    pVarchar->len = 0;
  };
  
  free(pVarchar);
};

// compare the contents of a string with the contents of our data
// if (varcharCmp(myVarchar, "Hello world") == 0) {
//   // myVarchar contains "Hello world"
// };
int varcharCmp(varchar * pVarchar, const char * pWith) {
  if (pVarchar == NULL) {
    return strlen(pWith);
  } else if (pVarchar->text == NULL) {
    return strlen(pWith);    
  };
  
  return strcmp(pVarchar->text, pWith);
};

// find the string pFind in our text and return the position
// pos = varcharPos(myVarchar, "world", 0);
// returns 7 if myVarchar contains "Hello world"
unsigned int varcharPos(varchar * pVarchar, const char * pFind, unsigned int pSearchFrom) {
  char * str;
  if (pVarchar == NULL) {
    return VARCHAR_NOTFOUND;
  } else if (pVarchar->text == NULL) {
    return VARCHAR_NOTFOUND;
  } else if (strlen(pFind) == 0) {
    return VARCHAR_NOTFOUND;
  } else if (pSearchFrom >= pVarchar->len) {
    return VARCHAR_NOTFOUND;    
  };
  
  str = strstr(pVarchar->text + pSearchFrom, pFind);
  if (str == NULL) {
    return VARCHAR_NOTFOUND;
  } else {
    return str - pVarchar->text;
  };
};

// return a substring, note that this returns a new object that needs to be release
// varchar * substring = varcharMid(myVarchar, 7, 5);
// // substring will now contain "world" if myVarchar was set to "Hello world"
// ...
// varcharRelease(substring); // don't forget to release it once we're done!!
varchar * varcharMid(varchar * pVarchar, unsigned int pStart, unsigned int pLen) {
  varchar * newChar = newVarchar();

  if (pVarchar == NULL) {
    // ignore
  } else if (pVarchar->text == NULL) {
    // ignore
  } else if (pStart >= pVarchar->len) {
    // ignore
  } else if (newChar != NULL) {
    if (pStart + pLen > pVarchar->len) {
      pLen = pVarchar->len - pStart;
    };
    
    varcharAppend(newChar, pVarchar->text + pStart, pLen);
  };
  
  return newChar;
};

// append a string to our buffer
// varchar * myVarchar = newVarchar();
// varcharAppend(myVarchar, "Hello");
// varcharAppend(myVarchar, " world");
// // myVarchar is now set to "Hello world"
// ...
// varcharRelease(myVarchar); // don't forget to release it once we're done!!
bool varcharAppend(varchar * pVarchar, const char * pText, unsigned int pLen) {
  if (pVarchar == NULL) {
    return false;    
  } else if (pLen == 0) {
    // nothing to add
    return true;
  } else if (!checkSize(pVarchar, pVarchar->len+pLen)) {
    /* we don't have enough space */
    return false;
  };
  
  memcpy(&(pVarchar->text[pVarchar->len]), pText, pLen);
  pVarchar->len += pLen;
  pVarchar->text[pVarchar->len] = '\0'; // zero terminate
    
  return true;
};

// Removes spaces and tabs from the start of our string
// varchar * myVarchar = newVarchar();
// varcharAppend(myVarchar, "   Hello world   ");
// varcharTrim(myVarchar);
// // myVarchar is now set to "Hello world"
// ...
// varcharRelease(myVarchar); // don't forget to release it once we're done!!

void varcharTrim(varchar * pVarchar) {
  int i;
  if (pVarchar == NULL) {
    // ??
  } else if (pVarchar->text == NULL) {
    // ignore
  } else {
    // while the first character is a space/tab, remove it
    while ((pVarchar->text[0] == ' ') || (pVarchar->text[0] == '\t')) {
      for (i = 1; i <= pVarchar->len; i++) { // <= means we also copy our 0 terminator!
        pVarchar->text[i-1] = pVarchar->text[i];
      };
      pVarchar->len--;
    };
    
    // while the last character is a space/tab remove it
    while ((pVarchar->len > 0) && ((pVarchar->text[pVarchar->len > 0 ? pVarchar->len - 1 : 0] == ' ') || (pVarchar->text[pVarchar->len > 0 ? pVarchar->len - 1 : 0] == '\t'))) {
      pVarchar->len--;
      pVarchar->text[pVarchar->len] = '\0';
    };
  };
};

#endif /* VARCHAR_IMPLEMENTATION */

#endif /* !varcharh */