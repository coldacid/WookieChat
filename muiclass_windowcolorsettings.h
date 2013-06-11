/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWCOLORSETTINGS_H
#define MUICLASS_WINDOWCOLORSETTINGS_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define WindowColorSettingsObject NewObject( appclasses[ CLASSID_WINDOWCOLORSETTINGS ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_WINDOWCOLORSETTINGS_LOADCOLORS =  0xFED00100,
MM_WINDOWCOLORSETTINGS_SAVECOLORS,
MM_WINDOWCOLORSETTINGS_CREATEDIR,

MM_WINDOWCOLORSETTINGS_UPDATECOLORS,
/* Attributes */
};

struct MP_WINDOWCOLORSETTINGS_LOADCOLORS { ULONG MethodID; STRPTR Name; };
struct MP_WINDOWCOLORSETTINGS_SAVECOLORS { ULONG MethodID; STRPTR Name; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowColorSettings_InitClass( void );
void    MCC_WindowColorSettings_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWCOLORSETTINGS_H */

