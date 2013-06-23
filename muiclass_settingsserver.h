/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SETTINGSSERVER_H
#define MUICLASS_SETTINGSSERVER_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"


#define SettingsServerObject NEWOBJECT( appclasses[ CLASSID_SETTINGSSERVER ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSSERVER_RESETTODEFAULTS =  0xFED01600,
MM_SETTINGSSERVER_READCONFIG,
MM_SETTINGSSERVER_WRITECONFIG,
MM_SETTINGSSERVER_DISENABLE,
MM_SETTINGSSERVER_SERVERGADGETSTOLIST,
MM_SETTINGSSERVER_SERVERLISTTOGADGETS,
MM_SETTINGSSERVER_NICKADD,
MM_SETTINGSSERVER_NICKREMOVE,
MM_SETTINGSSERVER_NICKGADGETSTOLIST,
MM_SETTINGSSERVER_NICKLISTTOGADGETS,
MM_SETTINGSSERVER_CHANNELADD,
MM_SETTINGSSERVER_CHANNELREMOVE,
MM_SETTINGSSERVER_CHANNELGADGETSTOLIST,
MM_SETTINGSSERVER_CHANNELLISTTOGADGETS,
/* Attributes */
};

struct MP_SETTINGSSERVER_READCONFIG { ULONG MethodID; ULONG ObjectID; };
struct MP_SETTINGSSERVER_WRITECONFIG { ULONG MethodID; ULONG ObjectID; APTR Data; };

/*************************************************************************/

#define SERVERENTRY_USERNAME_SIZEOF 32
#define SERVERENTRY_REALNAME_SIZEOF 32

/*
** Prototypes
*/

ULONG   MCC_SettingsServer_InitClass( void );
void    MCC_SettingsServer_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_SETTINGSSERVER_H */

