/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWIGNORELIST_H
#define MUICLASS_WINDOWIGNORELIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define WindowIgnoreListObject NewObject( appclasses[ CLASSID_WINDOWIGNORELIST ]->mcc_Class, NULL

/*
** ignore struct
*/
#define IGNORE_PATTERN_SIZEOF 0x40

struct Ignore {
	ULONG          Ignore_Flags;
	char           Ignore_Pattern[ IGNORE_PATTERN_SIZEOF + 2 ];
};

#define IGNOREF_PRIVMSG 0x1
#define IGNOREF_CTCP    0x2
#define IGNOREF_DCC     0x4

/*
** Methods and attributes
*/

enum {
MM_WINDOWIGNORELIST_DUMMY =  0xFED000e0,
MM_WINDOWIGNORELIST_DISENABLE,
MM_WINDOWIGNORELIST_GADGETSTOLIST,
MM_WINDOWIGNORELIST_LISTTOGADGETS,
MM_WINDOWIGNORELIST_LOADLIST,
MM_WINDOWIGNORELIST_SAVELIST,
MM_WINDOWIGNORELIST_ADD,
MM_WINDOWIGNORELIST_CHECKIGNORE,
/* Attributes */
};

struct MP_WINDOWIGNORELIST_CHECKIGNORE { ULONG MethodID; STRPTR Nick; STRPTR HostName; ULONG Type; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowIgnoreList_InitClass( void );
void    MCC_WindowIgnoreList_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWIGNORELIST_H */

