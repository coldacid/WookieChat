/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_ALIASLIST_H
#define MUICLASS_ALIASLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define AliasListObject NEWOBJECT( appclasses[ CLASSID_ALIASLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_ALIASLIST_ADD =  0xFED00100,
MM_ALIASLIST_IMPORTLISTASTEXT,
MM_ALIASLIST_EXPORTLISTASTEXT,
/* Attributes */
};

struct MP_ALIASLIST_ADD              { ULONG MethodID; STRPTR Alias; STRPTR Text; };
struct MP_ALIASLIST_IMPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };
struct MP_ALIASLIST_EXPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_AliasList_InitClass( void );
void    MCC_AliasList_DisposeClass( void );


/*************************************************************************/

/*
** alias entry structure
*/

#define ALIASENTRY_ALIAS_SIZEOF 16
#define ALIASENTRY_TEXT_SIZEOF  256

struct AliasEntry {
	unsigned char ae_Alias[ ALIASENTRY_ALIAS_SIZEOF + 2 ];
	unsigned char ae_Text[ ALIASENTRY_TEXT_SIZEOF + 2 ];
};

/*************************************************************************/

#endif /* MUICLASS_ALIASLIST_H */

