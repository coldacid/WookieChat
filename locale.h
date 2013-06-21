/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef LOCALE_H
#define LOCALE_H 1

/*************************************************************************/

#include <exec/types.h>
#include <dos/dos.h>
#include <libraries/locale.h>

#define CATCOMP_NUMBERS
#include "WookieChat_strings.h"

#define MSG_ERROR_NOERROR 0

/*
** Prototypes
*/

BOOL   Locale_Open( STRPTR catname, ULONG version, ULONG revision);
void   Locale_Close(void);
STRPTR Locale_GetString(long ID);
ULONG  Locale_FormatDate( char *format, char *target, struct DateStamp *ds, ULONG targetsize );

#define LGS( id ) Locale_GetString( id )

/*************************************************************************/

#endif /* LOCALE_H */
