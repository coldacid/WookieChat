/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSNICK_H
#define MUICLASS_SETTINGSNICK_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsNickObject NEWOBJECT( appclasses[ CLASSID_SETTINGSNICK ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSNICK_RESETTODEFAULTS =  0xFED00240,
MM_SETTINGSNICK_READCONFIG,
/* Attributes */
};

struct MP_SETTINGSNICK_READCONFIG { ULONG MethodID; ULONG ObjectID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsNick_InitClass( void );
void    MCC_SettingsNick_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSNICK_H */

