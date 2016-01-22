/********************************************************
 * errorlog.h - logging library by Bastiaan Olij 2016
 * 
 * Public domain, use as you say fit, disect, change,
 * or otherwise, all at your own risk
 *
 * This library is given as a single file implementation.
 * Include this in any file that requires it but in one
 * file, and one file only, proceed it with:
 * #define ERRORLOG_IMPLEMENTATION
 *
 * Revision history:
 * 0.1  17-01-2016  First version with basic functions
 *
 ********************************************************/

#ifndef errorlogh
#define errorlogh

// include some standard libraries
#ifdef __APPLE__
#include <syslog.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void errorlog(int error, const char* description, ...);

#ifdef __cplusplus
};
#endif 

#ifdef ERRORLOG_IMPLEMENTATION

// function for logging errors
void errorlog(int error, const char* description, ...) {
  va_list args;
  char fulldesc[2048];
  
  va_start(args, description);
  vsprintf(fulldesc, description, args);
  va_end(args);
  
#ifdef __APPLE__
	// output to syslog
	syslog(LOG_ALERT, "%i: %s", error, fulldesc);  
#elif WIN32
  // output to stderr
  fprintf(stderr, "%i: %s", error, fulldesc);
#else
  // not sure what we're doing on other platforms yet
#endif
};

#endif /* ERRORLOG_IMPLEMENTATION */

#endif /* !errorlogh */
