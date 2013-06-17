/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_APPLICATION_H
#define MUICLASS_APPLICATION_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

/*
** Methods and attributes
*/

enum {
MM_APPLICATION_DUMMY =  0xFED00000,
MM_APPLICATION_STARTUP,
/* Attributes */
MA_APPLICATION_OBJECTNETWORK,
MA_APPLICATION_WINDOWMAIN,
MA_APPLICATION_WINDOWQUIT,
MA_APPLICATION_WINDOWABOUT,
MA_APPLICATION_WINDOWSETTINGS,
MA_APPLICATION_WINDOWIGNORELIST,
MA_APPLICATION_WINDOWURLGRABBER,
};

/*
** some defines
*/

#ifndef MUIA_Application_UsedClasses
#define MUIA_Application_UsedClasses 0x8042e9a7
#endif

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_Application_InitClass( void );
void    MCC_Application_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_APPLICATION_H */

