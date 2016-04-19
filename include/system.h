/********************************************************
 * system.h - some system based functions by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define SYS_IMPLEMENTATION
 *
 * This library implements a few system facing functions
 * to make it easy to replace this code without having to
 * redo most of the libraries in this collection.
 *
 * Revision history:
 * 0.1  17-01-2016  First version with basic functions
 * 0.2  17-02-2016  Added our load function and renamed
 *                  our library
 *
 ********************************************************/

#ifndef systemh
#define systemh

// include some standard libraries
#ifdef __APPLE__
#include <syslog.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void errorlog(int error, const char* description, ...);
char* loadFile(const char* pPath, const char* pFileName);

#ifdef __cplusplus
};
#endif 

#ifdef SYS_IMPLEMENTATION

// function for logging errors
void errorlog(int error, const char* description, ...) {
  va_list args;
  char fulldesc[2048];
  FILE * log;
  
  va_start(args, description);
  vsprintf(fulldesc, description, args);
  va_end(args);
  
#ifdef __APPLE__
	// output to syslog
	syslog(LOG_ALERT, "%i: %s", error, fulldesc);  
#elif WIN32
  // output to file
  log = fopen("app.log", "a+");
  if (log != NULL) {
    fprintf(log, "%i: %s\n", error, fulldesc);
	fclose(log);
  };
#else
  // not sure what we're doing on other platforms yet
#endif
};

// load contents of a file
// return NULL on failure
// returns string on success, calling function is responsible for freeing the text
char* loadFile(const char* pPath, const char* pFileName) {
  char  error[1024];
  char* result = NULL;
  char  fileName[1024];

  // read "binary", we simply keep our newlines as is
  sprintf(fileName,"%s%s",pPath, pFileName);
  FILE* file = fopen(fileName,"rb");
  if (file == NULL) {
    errorlog(-100,"Couldn't open %s",fileName);
  } else {
    long size;
    
    // find the end of the file to get its size
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // allocate space for our data
    result = (char*) malloc(size+1);
    if (result == NULL) {
      errorlog(-101,"Couldn't allocate memory buffer for %li bytes",size);
    } else {
      // and load our text
      fread(result, 1, size, file);
      result[size] = 0;
    };
    
    fclose(file);
  };
  
  return result;
};

// gets the portion of the line up to the specified delimiter(s)
// return NULL on failure or if there is no text
// returns string on success, calling function is responsible for freeing the text
char * delimitText(const char *pText, const char *pDelimiters) {
  int    len = 0;
  char * result = NULL;
  bool   found = false;
  int    delimiterCount;

  delimiterCount = strlen(pDelimiters) + 1; // always include our trailing 0 as a delimiter ;)

  while (!found) {
    int pos = 0;
    while ((!found) && (pos < delimiterCount)) {
      if (pText[len] == pDelimiters[pos]) {
        found = true;
      };
      pos++;
    };

    if (!found) {
      len++;
    };
  };

  if (len != 0) {
    result = malloc(len + 1);
    if (result != NULL) {
      memcpy(result, pText, len);
      result[len] = 0;
    };
  };

  return result;
};

#endif /* SYS_IMPLEMENTATION */

#endif /* !systemh */
