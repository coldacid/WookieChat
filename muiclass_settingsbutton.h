/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSBUTTON_H
#define MUICLASS_SETTINGSBUTTON_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsButtonObject NEWOBJECT( appclasses[ CLASSID_SETTINGSBUTTON ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSBUTTON_RESETTODEFAULTS =  0xFED00f00,
MM_SETTINGSBUTTON_READCONFIG,
MM_SETTINGSBUTTON_ADD,
MM_SETTINGSBUTTON_DISENABLE,
MM_SETTINGSBUTTON_GADGETSTOLIST,
MM_SETTINGSBUTTON_LISTTOGADGETS,
/* Attributes */
};

struct MP_SETTINGSBUTTON_READCONFIG { ULONG MethodID; ULONG ObjectID; };
struct MP_SETTINGSBUTTON_ADD        { ULONG MethodID; STRPTR Button; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsButton_InitClass( void );
void    MCC_SettingsButton_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSBUTTON_H */

