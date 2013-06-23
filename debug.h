/*
**  Debug V4.0
**
** 	(C)1991 - 2013 by Guido Mersmann
**
**  For details and usage read debug.c
**
**  DISTRIBUTION
**
**  The contents of this file is considered to be PD and free to
**  use in any kind of software as long as the original credits
**  stay intact.
**
*/

#ifndef DEBUG_DEBUG_H
#define DEBUG_DEBUG_H 1

/*************************************************************************/

#include <stdarg.h>
#include <exec/types.h>
#include <exec/initializers.h>

#include <proto/exec.h>

#include <SDI_compiler.h>

/*************************************************************************/

#if defined(DEBUG) && ( !defined(NODEBUG) || defined(FORCEDEBUG) )

/* This is our debug function */
void STDARGS debug( char *text, ...);

#else
    #ifdef __VBCC__
        #define debug(...) do{}while(0)
    #endif
    #ifdef __GNUC__
		#define debug(...) {}
    #endif
#endif

/*************************************************************************/

#endif /* DEBUG_DEBUG_H */

