/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWMAIN_H
#define MUICLASS_WINDOWMAIN_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define WindowMainObject NewObject( appclasses[ CLASSID_WINDOWMAIN ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_WINDOWMAIN_DUMMY =  0xFED00020,
MM_WINDOWMAIN_MENUSELECT,
MM_WINDOWMAIN_COLORCHANGE,
/* Attributes */
};

struct MP_WINDOWMAIN_MENUSELECT { ULONG MethodID; ULONG MenuID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowMain_InitClass( void );
void    MCC_WindowMain_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWMAIN_H */

