/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_SERVERLIST_H
#define MUICLASS_SERVERLIST_H 1

/*************************************************************************/

#include <exec/types.h>
#include "muiclass.h"

#define ServerListObject NEWOBJECT( appclasses[ CLASSID_SERVERLIST ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_SERVERLIST_ADD =  0xFED00370,
MM_SERVERLIST_ACTIVECHANGE,
MM_SERVERLIST_IMPORTLISTASTEXT,
MM_SERVERLIST_EXPORTLISTASTEXT,
/* Attributes */
MA_SERVERLIST_CHANNELLISTOBJ,
MA_SERVERLIST_NICKLISTOBJ,
};

struct MP_SERVERLIST_ADD              { ULONG MethodID; STRPTR Name; STRPTR Address; ULONG Port; STRPTR Password; STRPTR Charset; ULONG Flags;  };
struct MP_SERVERLIST_IMPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };
struct MP_SERVERLIST_EXPORTLISTASTEXT { ULONG MethodID; STRPTR Name; };

/*************************************************************************/

/*
** Prototypes
*/

ULONG   MCC_ServerList_InitClass( void );
void    MCC_ServerList_DisposeClass( void );


/*************************************************************************/

/*
** server entry structure
*/

#define SERVERENTRY_NAME_SIZEOF       16
#define SERVERENTRY_ADDRESS_SIZEOF   254
#define SERVERENTRY_CHARSET_SIZEOF    30
#define SERVERENTRY_PASSWORD_SIZEOF   30

struct ServerEntry {
	ULONG         se_Port;
	ULONG         se_Flags;
	struct List   se_NickList;
	struct List   se_ChannelList;
	char          se_Name[ SERVERENTRY_NAME_SIZEOF + 2 ];
	char          se_Address[ SERVERENTRY_ADDRESS_SIZEOF + 2 ];
	char          se_Charset[ SERVERENTRY_CHARSET_SIZEOF + 2 ];
	char          se_Password[ SERVERENTRY_PASSWORD_SIZEOF + 2 ];
};

#define SERVERENTRYF_AUTOCONNECT 1

/*************************************************************************/

#endif /* MUICLASS_SERVERLIST_H */

