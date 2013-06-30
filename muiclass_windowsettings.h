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

/*************************************************************************/

#define WindowSettingsObject NEWOBJECT( appclasses[ CLASSID_WINDOWSETTINGS ]->mcc_Class, NULL

/*************************************************************************/

/*
** Methods and attributes
*/

enum {
MM_WINDOWSETTINGS_READCONFIG =  0xFED00050,
MM_WINDOWSETTINGS_WRITECONFIG,
MM_WINDOWSETTINGS_SAVE,
MM_WINDOWSETTINGS_USE,
MM_WINDOWSETTINGS_CANCEL,
/* Attributes */
MA_WINDOWSETTINGS_VISUALCHANGE,
};

struct MP_WINDOWSETTINGS_READCONFIG  { ULONG MethodID; ULONG ObjectID; };
struct MP_WINDOWSETTINGS_WRITECONFIG { ULONG MethodID; ULONG ObjectID; APTR Data; };

/*************************************************************************/

/*
** path data
*/

#define DEFAULT_SETTINGSPATH            "PROGDIR:Prefs"
#define DEFAULT_SETTINGSNICK_NAME       DEFAULT_SETTINGSPATH "/nick.prefs"
#define DEFAULT_SETTINGSCOLOR_NAME      DEFAULT_SETTINGSPATH "/color.prefs"
#define DEFAULT_SETTINGSIGNORE_NAME     DEFAULT_SETTINGSPATH "/ignore.prefs"
#define DEFAULT_SETTINGSURL_NAME        DEFAULT_SETTINGSPATH "/url.prefs"
#define DEFAULT_SETTINGSSERVER_NAME     DEFAULT_SETTINGSPATH "/server.prefs"

#define DEFAULT_PRESETSPATH             "PROGDIR:Presets"
#define DEFAULT_PRESETSSOUNDSPATH       DEFAULT_PRESETSPATH "/SoundEffects"
#define DEFAULT_PRESETSCOLORSPATH       DEFAULT_PRESETSPATH "/Colors"

#ifdef __MORPHOS__
#define DEFAULT_SERVER_NAME      "Freenode"
#define DEFAULT_SERVER_ADDRESS   "irc.freenode.net"
#define DEFAULT_SERVER_PORT      6667
#define DEFAULT_SERVER_CHARSET   "UFT-8"
#define DEFAULT_SERVER_PASSWORD  ""
#define DEFAULT_CHANNEL_NAME     "#morphos"
#define DEFAULT_CHANNEL_PASSWORD ""
#elif  __amigaos4__
#define DEFAULT_SERVER_NAME      "AmigaWorld"
#define DEFAULT_SERVER_ADDRESS   "chat.amigaworld.net"
#define DEFAULT_SERVER_PORT      6667
#define DEFAULT_SERVER_CHARSET   "UFT-8"
#define DEFAULT_SERVER_PASSWORD  ""
#define DEFAULT_CHANNEL_NAME     "#amigaworld"
#define DEFAULT_CHANNEL_PASSWORD ""
#elif  __AROS__
#define DEFAULT_SERVER_NAME      "Freenode"
#define DEFAULT_SERVER_ADDRESS   "irc.freenode.net"
#define DEFAULT_SERVER_PORT      6667
#define DEFAULT_SERVER_CHARSET   "UFT-8"
#define DEFAULT_SERVER_PASSWORD  ""
#define DEFAULT_CHANNEL_NAME     "#aros"
#define DEFAULT_CHANNEL_PASSWORD ""
#else
#define DEFAULT_SERVER_NAME      "Freenode"
#define DEFAULT_SERVER_ADDRESS   "irc.freenode.net"
#define DEFAULT_SERVER_PORT      6667
#define DEFAULT_SERVER_CHARSET   "UFT-8"
#define DEFAULT_SERVER_PASSWORD  ""
#define DEFAULT_CHANNEL_NAME     "#amiganews.de"
#define DEFAULT_CHANNEL_PASSWORD ""
#endif
/*************************************************************************/

/*
** settings tab numbering
*/

enum{
SETTINGS_SERVER = 0,
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


/*************************************************************************/

/*
** This is the macro to obtain any
** configuration at runtime.
**
** As argument use the OIDs below
*/

#define GRC(a) GlobalReadConfig(a)
#define GRCO(o,a) GlobalReadConfigObj(o,a)

#define LocalReadConfig(a) DoMethod( mccdata->mcc_ClassObjects[ WID_SETTINGS ], MM_WINDOWSETTINGS_READCONFIG, a )
#define LRC(a) LocalReadConfig(a)

/*************************************************************************/

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
_OID_SETTINGSNICK = 100,
/* server settings */
OID_SETTINGSSERVER = 200,
OID_SVR_LIST,         /* just returns the list object (WIZARD ONLY) */
OID_SVR_NICKLIST,     /* just returns the list object (WIZARD ONLY) */
OID_SVR_CHANNELLIST,  /* just returns the list object (WIZARD ONLY) */
OID_SVR_REALNAME,
OID_SVR_USERNAME,
OID_SVR_RECONNECT,
OID_SVR_RECONNECTDELAY,
OID_SVR_DUPEDETECTION,
/* gui settings */
OID_SETTINGSGUI = 300,
OID_GUI_TABBUTTON,
OID_GUI_TABSERVER,
OID_GUI_NICKLISTWIDTH,
OID_GUI_NICKLISTHEIGHT,
OID_GUI_NICKLISTGFXINFO,
OID_GUI_NICKLISTREMCHARS,
OID_GUI_SMILIES,
OID_GUI_SMILIESTHEME,
OID_GUI_TIMESHOW,
OID_GUI_TIMEFORMAT,
OID_GUI_SHOWJOINPART,
OID_GUI_CHANNELVIEWNICKWIDTH,
OID_GUI_CHANNELVIEWSPACING,
/* logging settings */
OID_SETTINGSLOGGING = 400,
OID_LOG_ACTIVEPUBLIC,
OID_LOG_ACTIVEPRIVATE,
OID_LOG_RECOVERPUBLIC,
OID_LOG_RECOVERPRIVATE,
OID_LOG_SPLIT,
OID_LOG_SPLITLIMIT,
/* dcc settings */
OID_SETTINGSDCC = 500,
OID_DCC_USELOCALADDRESS,
OID_DCC_USECUSTOMADDRESS,
OID_DCC_CUSTOMADDRESS,
OID_DCC_RANGEFROM,
OID_DCC_RANGETO,
OID_DCC_INPATH,
OID_DCC_INACCEPT,
OID_DCC_INWRITEMODE,
OID_DCC_OUTPATH,
/* sound settings */
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
OID_SND_ENABLE,
/* alias settings */
OID_SETTINGSALIAS = 700,
/* button settings */
OID_SETTINGSBUTTONS = 800,
OID_BUT_SHOWBUTTONS,
/* events settings */
OID_SETTINGSEVENTS = 900,
/* color settings */
OID_SETTINGSCOLOR = 1000, /* this is the PEN index, add non pen colors at 1100,
so one can add more colors if needed */
OID_COL_OTHERDATA = 1100,
/* alias settings */
OID_ALI_LIST = 0x1000,
OID_BUT_LIST = 0x2000,
OID_EVT_LIST = 0x3000,
OID_IGN_LIST = 0x4000,
OID_URL_LIST = 0x5000,
};

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_WindowSettings_InitClass( void );
void    MCC_WindowSettings_DisposeClass( void );
ULONG   GlobalReadConfig( ULONG objectid );
ULONG   GlobalReadConfigObj( Object *obj, ULONG objectid );

/*************************************************************************/

#endif /* MUICLASS_WINDOWSETTINGS_H */

