/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_URLLIST_H
#define MUICLASS_URLLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define URLListObject NEWOBJECT( appclasses[ CLASSID_URLLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_URLLIST_ADD =  0xFED00390,
MM_URLLIST_IMPORTLISTASTEXT,
MM_URLLIST_EXPORTLISTASTEXT,
/* Attributes */
};

struct MP_URLLIST_ADD              { ULONG MethodID; STRPTR URL; };
struct MP_URLLIST_IMPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };
struct MP_URLLIST_EXPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_URLList_InitClass( void );
void    MCC_URLList_DisposeClass( void );


/*************************************************************************/

/*
** url entry structure
*/

#define URLENTRY_URL_SIZEOF 0x200

struct URLEntry {
	unsigned char ue_URL[ URLENTRY_URL_SIZEOF + 2 ];
};

/*************************************************************************/

#endif /* MUICLASS_URLLIST_H */

