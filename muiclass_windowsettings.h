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
** This is the macro to obtain any
** configuration at runtime.
**
** As argument use the OIDs below
*/

#define GRC(a) GlobalReadConfig(a)

/*
** object ids
*/

enum{
/* general settings */
OID_SETTINGSGENERAL = 0,
OID_GEN_EDITOR,
OID_GEN_BROWSER,
OID_GEN_MSGKICK,
OID_GEN_MSGQUIT,
OID_GEN_MSGPART,
OID_GEN_OPENPRIVATEQUERY,
OID_GEN_REQUESTWHOIS,
OID_GEN_REJOIN,
OID_GEN_OPENQUERY,
OID_GEN_HIGHLIGHTPATTERN,
OID_GEN_NICKCOMPLETITION,
/* nick settings */
OID_SETTINGSNICK = 100,
OID_NIK_NICK,
OID_NIK_NICK1,
OID_NIK_NICK2,
OID_NIK_USERNAME,
OID_NIK_REALNAME,
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
OID_SND_MODEONTABOPENING,
OID_SND_SPLONTABOPENING,
OID_SND_MODEONHIGHLIGHT,
OID_SND_SPLONHIGHLIGHT,
OID_SND_MODEONPRIVMSG,
OID_SND_SPLONPRIVMSG,
OID_SND_CTCPSAMPLES,
OID_SND_USEEXTERNALPLAYER,
OID_SND_EXTERNALPLAYER,
/* logging alias */
OID_SETTINGSALIAS = 700,
/* logging button */
OID_SETTINGSBUTTONS = 800,
/* logging events */
OID_SETTINGSEVENTS = 900,
/* color settings */
OID_SETTINGSCOLOR = 1000,
OID_COL_BACKGROUND,
OID_COL_NICKLISTBG,
OID_COL_TABLISTBG,
OID_COL_NORMAL,
OID_COL_NICKLISTTX,
OID_COL_TABSPEN,
OID_COL_JOIN,
OID_COL_PART,
OID_COL_QUIT,
OID_COL_MODES,
OID_COL_CTCP,
OID_COL_ACTIONS,
OID_COL_INFO,
OID_COL_OWNTEXT,
OID_COL_HIGHLIGHT,
OID_COL_NOTICES,
OID_COL_INVITE,
OID_COL_KICK,
OID_COL_NICKCHANGE,
OID_COL_TOPIC,
OID_COL_WALLOPS,
OID_COL_ACTIVITY,
OID_COL_CHATACTIVITY,
OID_COL_HIGHLIGHTACTIVITY,
};

/*
** Methods and attributes
*/

enum {
MM_WINDOWSETTINGS_READCONFIG =  0xFED000c0,
/* Attributes */
};

struct MP_WINDOWSETTINGS_READCONFIG { ULONG MethodID; ULONG ObjectID; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowSettings_InitClass( void );
void    MCC_WindowSettings_DisposeClass( void );
ULONG   GlobalReadConfig( ULONG objectid );

/*************************************************************************/

#endif /* MUICLASS_WINDOWSETTINGS_H */

