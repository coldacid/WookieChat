/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_WINDOWSETTINGS_H
#define MUICLASS_WINDOWSETTINGS_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define WindowSettingsObject NewObject( appclasses[ CLASSID_WINDOWSETTINGS ]->mcc_Class, NULL

enum{
SETTINGS_NICK = 0,
SETTINGS_SERVER,
SETTINGS_GENERAL,
SETTINGS_COLOR,
SETTINGS_GUI,
SETTINGS_LOGGING,
SETTINGS_DCC,
SETTINGS_SOUNDS,
SETTINGS_ALIASES,
SETTINGS_BUTTONS,
SETTINGS_EVENTS,
SETTINGS_LAST,
};

/*
** object ids
*/

enum{
/* general settings */
OID_SETTINGSGENERAL = 0,
OID_EDITOR,
OID_BROWSER,
OID_MSGKICK,
OID_MSGQUIT,
OID_MSGPART,
OID_OPENPRIVATEQUERY,
OID_REQUESTWHOIS,
OID_REJOIN,
OID_OPENQUERY,
OID_HIGHLIGHTPATTERN,
OID_NICKCOMPLETITION,
/* nick settings */
OID_SETTINGSNICK = 100,
OID_NICK,
OID_NICK1,
OID_NICK2,
OID_USERNAME,
OID_REALNAME,
/* server settings */
OID_SETTINGSSERVER = 200,
/* server gui */
OID_SETTINGSGUI = 300,
/* logging settings */
OID_SETTINGSLOGGING = 400,
/* logging dcc */
OID_SETTINGSDCC = 500,
/* logging sound */
OID_SETTINGSSOUND = 600,
/* logging alias */
OID_SETTINGSALIAS = 700,
/* logging button */
OID_SETTINGSBUTTONS = 800,
/* logging events */
OID_SETTINGSEVENTS = 900,
/* color settings */
OID_SETTINGSCOLOR = 1000,
OID_BACKGROUND,
OID_NICKLISTBG,
OID_TABLISTBG,
OID_NORMAL,
OID_NICKLISTTX,
OID_TABSPEN,
OID_JOIN,
OID_PART,
OID_QUIT,
OID_MODES,
OID_CTCP,
OID_ACTIONS,
OID_INFO,
OID_OWNTEXT,
OID_HIGHLIGHT,
OID_NOTICES,
OID_INVITE,
OID_KICK,
OID_NICKCHANGE,
OID_TOPIC,
OID_WALLOPS,
OID_ACTIVITY,
OID_CHATACTIVITY,
OID_HIGHLIGHTACTIVITY,
};

/*
** Methods and attributes
*/

enum {
MM_WINDOWSETTINGS_DUMMY =  0xFED000c0,
/* Attributes */
};

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowSettings_InitClass( void );
void    MCC_WindowSettings_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_WINDOWSETTINGS_H */

