/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_IGNORELIST_H
#define MUICLASS_IGNORELIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define IgnoreListObject NEWOBJECT( appclasses[ CLASSID_IGNORELIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_IGNORELIST_ADD =  0xFED00380,
MM_IGNORELIST_IMPORTLISTASTEXT,
MM_IGNORELIST_EXPORTLISTASTEXT,
/* Attributes */
};

struct MP_IGNORELIST_ADD              { ULONG MethodID; STRPTR Pattern; ULONG Flags; };
struct MP_IGNORELIST_IMPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };
struct MP_IGNORELIST_EXPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_IgnoreList_InitClass( void );
void    MCC_IgnoreList_DisposeClass( void );


/*************************************************************************/

/*
** ignore entry structure
*/

#define IGNOREENTRY_PATTERN_SIZEOF 0x40

struct IgnoreEntry {
	ULONG         ie_Flags;
	unsigned char ie_Pattern[ IGNOREENTRY_PATTERN_SIZEOF + 2 ];
};

#define IGNOREENTRYF_TEXT    0x1
#define IGNOREENTRYF_CTCP    0x2
#define IGNOREENTRYF_DCC     0x4

/*************************************************************************/

#endif /* MUICLASS_IGNORELIST_H */

