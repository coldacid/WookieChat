/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSLOG_H
#define MUICLASS_SETTINGSLOG_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsLogObject NEWOBJECT( appclasses[ CLASSID_SETTINGSLOG ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSLOG_RESETTODEFAULTS =  0xFED00240,
MM_SETTINGSLOG_READCONFIG,
/* Attributes */
};

struct MP_SETTINGSLOG_READCONFIG { ULONG MethodID; ULONG ObjectID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_SettingsLog_InitClass( void );
void    MCC_SettingsLog_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSLOG_H */

