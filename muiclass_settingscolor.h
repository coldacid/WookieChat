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


#define SettingsColorObject NEWOBJECT( appclasses[ CLASSID_SETTINGSCOLOR ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SETTINGSCOLOR_DUMMY =  0xFED01000,
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

/*
** Pens
*/

enum{
PEN_CHANNELLISTBACKGROUND = 0,
PEN_CHANNELLISTTEXT,
PEN_CHANNELLISTUSER,
PEN_CHANNELLISTSERVER,
PEN_CHANNELLISTHIGHLIGHT,
PEN_USERLISTBACKGROUND,
PEN_USERLISTTEXT,
PEN_LOGLISTBACKGROUND,
PEN_LOGPRIVMSG,
PEN_LOGJOIN,
PEN_LOGPART,
PEN_LOGQUIT,
PEN_LOGMODE,
PEN_LOGCTCP,
PEN_LOGACTION,
PEN_LOGINFO,
PEN_LOGOWNTEXT,
PEN_LOGHIGHLIGHT,
PEN_LOGNOTICE,
PEN_LOGINVITE,
PEN_LOGKICK,
PEN_LOGNICKCHANGE,
PEN_LOGTOPIC,
PEN_LOGWALLOPS,
PEN_NUMBEROF
};

/*************************************************************************/

#endif /* MUICLASS_SETTINGSCOLOR_H */

