/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_NICKLIST_H
#define MUICLASS_NICKLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define NickListObject NEWOBJECT( appclasses[ CLASSID_NICKLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_NICKLIST_ADD =  0xFED00300,
MM_NICKLIST_REMOVE,
/* Attributes */
};

struct MP_NICKLIST_ADD    { ULONG MethodID; APTR ServerEntry; STRPTR Nick; STRPTR Password; };
struct MP_NICKLIST_REMOVE { ULONG MethodID; APTR NickEntry; };


/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_NickList_InitClass( void );
void    MCC_NickList_DisposeClass( void );

/*
** nick entry structure
*/

#define NICKENTRY_NICK_SIZEOF     30
#define NICKENTRY_PASSWORD_SIZEOF 30

struct NickEntry {
	struct NickEntry    *ne_Succ;
	struct NickEntry    *ne_Pred;
	char                 ne_Nick[ NICKENTRY_NICK_SIZEOF + 2 ];
	char                 ne_Password[ NICKENTRY_PASSWORD_SIZEOF + 2 ];
};

/*************************************************************************/

#endif /* MUICLASS_NICKLIST_H */


