/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSGENERAL_H
#define MUICLASS_SETTINGSGENERAL_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsGeneralObject NEWOBJECT( appclasses[ CLASSID_SETTINGSGENERAL ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSGENERAL_RESETTODEFAULTS =  0xFED00220,
MM_SETTINGSGENERAL_READCONFIG,
/* Attributes */
};

struct MP_SETTINGSGENERAL_READCONFIG { ULONG MethodID; ULONG ObjectID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsGeneral_InitClass( void );
void    MCC_SettingsGeneral_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSGENERAL_H */

