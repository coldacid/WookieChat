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
MM_ALIASLIST_ADD =  0xFED00500,
/* Attributes */
};

struct MP_ALIASLIST_ADD { ULONG MethodID; STRPTR Alias; STRPTR Text; };

/*
** alias structure
*/

#define ALIAS_ALIAS_SIZEOF 16
#define ALIAS_TEXT_SIZEOF  256

struct Alias {
	unsigned char alias_Alias[ ALIAS_ALIAS_SIZEOF + 2 ];
	unsigned char alias_Text[ ALIAS_TEXT_SIZEOF + 2 ];
};

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_AliasList_InitClass( void );
void    MCC_AliasList_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_ALIASLIST_H */

