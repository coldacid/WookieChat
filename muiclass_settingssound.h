/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSSOUND_H
#define MUICLASS_SETTINGSSOUND_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsSoundObject NEWOBJECT( appclasses[ CLASSID_SETTINGSSOUND ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSSOUND_RESETTODEFAULTS =  0xFED01700,
MM_SETTINGSSOUND_READCONFIG,
MM_SETTINGSSOUND_DISENABLE,
/* Attributes */
};

struct MP_SETTINGSSOUND_READCONFIG { ULONG MethodID; ULONG ObjectID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsSound_InitClass( void );
void    MCC_SettingsSound_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSSOUND_H */

