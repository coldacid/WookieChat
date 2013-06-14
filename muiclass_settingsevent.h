/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSEVENT_H
#define MUICLASS_SETTINGSEVENT_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsEventObject NEWOBJECT( appclasses[ CLASSID_SETTINGSEVENT ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSEVENT_RESETTODEFAULTS =  0xFED00290,
MM_SETTINGSEVENT_READCONFIG,
MM_SETTINGSEVENT_ADD,
MM_SETTINGSEVENT_DISENABLE,
MM_SETTINGSEVENT_GADGETSTOLIST,
MM_SETTINGSEVENT_LISTTOGADGETS,
/* Attributes */
};

struct MP_SETTINGSEVENT_READCONFIG { ULONG MethodID; ULONG ObjectID; };
struct MP_SETTINGSEVENT_ADD        { ULONG MethodID; STRPTR Event; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsEvent_InitClass( void );
void    MCC_SettingsEvent_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSEVENT_H */

