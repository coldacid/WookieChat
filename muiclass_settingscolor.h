/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSCOLOR_H
#define MUICLASS_SETTINGSCOLOR_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsColorObject NewObject( appclasses[ CLASSID_SETTINGSCOLOR ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSCOLOR_DUMMY =  0xFED00200,
MM_SETTINGSCOLOR_RESETTODEFAULTS,
MM_SETTINGSCOLOR_READCONFIG,
/* Attributes */
};

struct MP_SETTINGSCOLOR_READCONFIG { ULONG MethodID; ULONG ObjectID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsColor_InitClass( void );
void    MCC_SettingsColor_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSCOLOR_H */

