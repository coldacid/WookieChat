/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSALIAS_H
#define MUICLASS_SETTINGSALIAS_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsAliasObject NEWOBJECT( appclasses[ CLASSID_SETTINGSALIAS ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSALIAS_RESETTODEFAULTS =  0xFED00e00,
MM_SETTINGSALIAS_READCONFIG,
MM_SETTINGSALIAS_ADD,
MM_SETTINGSALIAS_DISENABLE,
MM_SETTINGSALIAS_GADGETSTOLIST,
MM_SETTINGSALIAS_LISTTOGADGETS,
/* Attributes */
};

struct MP_SETTINGSALIAS_READCONFIG { ULONG MethodID; ULONG ObjectID; };
struct MP_SETTINGSALIAS_ADD        { ULONG MethodID; STRPTR Alias; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsAlias_InitClass( void );
void    MCC_SettingsAlias_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSALIAS_H */

