/********************************************************
 * linkedlist.h - generic linked list by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define LINKEDLIST_IMPLEMENTATION
 *
 * Revision history:
 * 0.1  15-01-2016  First version with basic functions
 *
 ********************************************************/

#ifndef linkedlisth
#define linkedlisth

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

typedef void(* dataRetainFunc)(void * pData);
typedef void(* dataFreeFunc)(void * pData);

#ifdef __cplusplus
};
#endif 

typedef struct llistNode {
  void *              data;           // pointer to data in our node
  struct llistNode *  prev;           // pointer to the previous entry in our linked list
  struct llistNode *  next;           // pointer to the next entry in our linked list
} llistNode;

typedef struct llist {
  llistNode *         first;          // pointer to our first entry in our linked list (NULL if empty)
  llistNode *         last;           // pointer to our last entry in our linked list (NULL if empty)
  dataRetainFunc      dataRetain;     // pointer to function that retains our data
  dataFreeFunc        dataFree;       // pointer to function that frees/releases up our data when we destruct our linked list (NULL = don't free)
  unsigned int        numEntries;     // number of entries contained in our list
} llist;

#ifdef __cplusplus
extern "C" {
#endif
  
llist * newLlist(dataRetainFunc pDataRetain, dataFreeFunc pDataFree);
void llistFree(llist * pList);
bool llistAddTo(llist * pList, void * pData);
bool llistRemove(llist * pList, void * pData);
bool llistMerge(llist * pList, llist * pMergeWith);
void * llistDataAtIndex(llist * pList, unsigned int pIndex);

#ifdef __cplusplus
};
#endif 

#ifdef LINKEDLIST_IMPLEMENTATION

// creates a new linked list
// llist * myList = newLlist((dataRetainFunc) myRetainFunc, (dataReleaseFunc) myReleaseFunc);
llist * newLlist(dataRetainFunc pDataRetain, dataFreeFunc pDataFree) {
  llist * newList = (llist *) malloc(sizeof(llist));
  if (newList != NULL) {
    newList->first = NULL;
    newList->last = NULL;
    newList->dataRetain = pDataRetain;
    newList->dataFree = pDataFree;
    newList->numEntries = 0;
  };
  return newList;
};

// frees the memory related to our linked list. If a release function was provided all objects are released.
// llistFree(myList);
void llistFree(llist * pList) {
  if (pList == NULL) {
    return;
  };
  
  llistNode * nextNode = pList->first;
  while (nextNode != NULL) {
    llistNode * currNode = nextNode;
    nextNode = currNode->next;
        
    // if we have a function to free/release up our data, then call that
    if ((currNode->data != NULL) && (pList->dataFree != NULL)) {
      pList->dataFree(currNode->data);
      currNode->data = NULL;
    };
    
    // now free our node
    free(currNode);
  };
  
  // and free our list
  free(pList);
};

// adds a node at the end of our linked list, if a retain function was provided the object is retained
// llistAddTo(myList, myObject);
bool llistAddTo(llist * pList, void * pData) {
  llistNode * newNode;

  if (pList == NULL) {
    return false;
  };

  // create our node
  newNode = (llistNode *) malloc(sizeof(llistNode));
  if (newNode == NULL) {
    return false;
  };
  newNode->data = pData;
  if ((pData != NULL) && (pList->dataRetain != NULL)) {
    // if we can retain our data we do so
    pList->dataRetain(pData);
  };
  newNode->prev = pList->last; // our last node becomes our previous node
  newNode->next = NULL; // we are the last node
  
  // if our first pointer is NULL, this is our first entry
  if (pList->first == NULL) {
    pList->first = newNode;    
  };
  
  if (pList->last != NULL) {
    // our last is no longer our last, have it point to our new node
    pList->last->next = newNode;
  };
  // we are our new last node
  pList->last = newNode;
  
//  syslog(LOG_ALERT, "Added node: %p, prev: %p, data: %p", newNode, newNode->prev, newNode->data); 
  
  pList->numEntries++;
  
  return true;
};

// removes a node from our linked list
// data is only freed if we're using a retain function
// llistRemove(myList, myObject);
bool llistRemove(llist * pList, void * pData) {
  llistNode * node;

  if (pList == NULL) {
    return false;
  };
  
  node = pList->first;
  while (node != NULL) {
    if (node->data == pData) {
      if (node->prev != NULL) {
        // our previous node now points to our next node
        node->prev->next = node->next;
      } else {
        // our next node becomes our first node
        pList->first = node->next;
      };
      
      if (node->next != NULL) {
        // our next node now points to our previous node
        node->next->prev = node->prev;
      } else {
        // our previous node becomes our last node
        pList->last = node->prev;
      };

      // and clean up
      if ((node->data != NULL) && (pList->dataRetain != NULL) && (pList->dataFree != NULL)) {
        pList->dataFree(node->data);
        node->data = NULL;
      };
      free(node);
      pList->numEntries--;
      return true;
    };
  
    // check our next node
    node = node->next;
  };
  
  return false;
};

// add the contents of one list to another list
// llistMerge(myList, myOtherList);
// llistFree(myOtherList); // this has now been added to our list, we can clean this up...
bool llistMerge(llist * pList, llist * pMergeWith) {  
  if (pList == NULL) {
    return false;
  };
  
  if (pMergeWith != NULL) {
    llistNode * node = pMergeWith->first;
   
    while (node != NULL) {
      if (!llistAddTo(pList, node->data)) {
        return false;
      };
      
      node = node->next;
    };
  }; 

  return true;
};

// return our data pointer at index, returns NULL if none is found
// mystruct * object = (mystruct *) llistDataAtIndex(myList, 15);
void * llistDataAtIndex(llist * pList, unsigned int pIndex) {
  bool reverse;
  unsigned int index;
  llistNode * node;

  if (pList == NULL) {
    return NULL;
  } else if (pList->numEntries == 0) {
    // no entries!
    return NULL;
  } else if (pIndex>=pList->numEntries) {
    // index out of bounds!
    return NULL;
  };
  
  // if the index we're looking for is in our second half of our list, faster to work backwards
  reverse = pIndex > (pList->numEntries / 2);
  if (reverse) {
    index = pList->numEntries - 1;
    node = pList->last;
  } else {
    index = 1;
    node = pList->first;
  };

  while (node != NULL) {
    // is this the node we're looking for?
    if (index == pIndex) {
      return node;
    };
    
    // check the previous/next node
    if (reverse) {
      node = node->prev;
      index--;      
    } else {
      node = node->next;
      index++;      
    };
  };
  
  return NULL;
};

#endif /* LINKEDLIST_IMPLEMENTATION */

#endif /* !linkedlisth */
