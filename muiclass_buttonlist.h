/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_BUTTONLIST_H
#define MUICLASS_BUTTONLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define ButtonListObject NEWOBJECT( appclasses[ CLASSID_BUTTONLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_BUTTONLIST_ADD =  0xFED00330,
/* Attributes */
};

struct MP_BUTTONLIST_ADD { ULONG MethodID; STRPTR Name; STRPTR Command; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_ButtonList_InitClass( void );
void    MCC_ButtonList_DisposeClass( void );


/*************************************************************************/

/*
** button entry structure
*/

#define BUTTONENTRY_NAME_SIZEOF     16
#define BUTTONENTRY_COMMAND_SIZEOF  256

struct ButtonEntry {
	unsigned char be_Name[ BUTTONENTRY_NAME_SIZEOF + 2 ];
	unsigned char be_Command[ BUTTONENTRY_COMMAND_SIZEOF + 2 ];
};

/*************************************************************************/

#endif /* MUICLASS_BUTTONLIST_H */

