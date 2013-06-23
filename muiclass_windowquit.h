/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWQUIT_H
#define MUICLASS_WINDOWQUIT_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define WindowQuitObject NEWOBJECT( appclasses[ CLASSID_WINDOWQUIT ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_WINDOWQUIT_DUMMY =  0xFED00040,
/* Attributes */
};

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowQuit_InitClass( void );
void    MCC_WindowQuit_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWQUIT_H */

