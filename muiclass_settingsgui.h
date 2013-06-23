/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSGUI_H
#define MUICLASS_SETTINGSGUI_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsGUIObject NEWOBJECT( appclasses[ CLASSID_SETTINGSGUI ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSGUI_RESETTODEFAULTS =  0xFED01400,
MM_SETTINGSGUI_READCONFIG,
/* Attributes */
};

struct MP_SETTINGSGUI_READCONFIG { ULONG MethodID; ULONG ObjectID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsGUI_InitClass( void );
void    MCC_SettingsGUI_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSGUI_H */

