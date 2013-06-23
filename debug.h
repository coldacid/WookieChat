/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
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

