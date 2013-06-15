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
/* Attributes */
};

struct MP_SERVERLIST_ADD { ULONG MethodID; ULONG Type; ULONG Mode; STRPTR Script; STRPTR Text; };

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

#define SERVERENTRY_ADDRESS_SIZEOF         254
#define SERVERENTRY_CHARSET_SIZEOF          30
#define SERVERENTRY_PASSWORDSERVER_SIZEOF   30
#define SERVERENTRY_PASSWORDNICK_SIZEOF     30
#define SERVERENTRY_AUTOJOIN_SIZEOF         30

struct ServerEntry {
	ULONG         se_Port;
	ULONG         se_Mode;
	struct List   se_NickList;
	struct List   se_ChannelList;
	char          se_Address[ SERVERENTRY_ADDRESS_SIZEOF + 2 ];
	char          se_Charset[ SERVERENTRY_CHARSET_SIZEOF + 2 ];
	char          se_PasswordServer[ SERVERENTRY_PASSWORDSERVER_SIZEOF + 2 ];
	char          se_PasswordNick[ SERVERENTRY_PASSWORDNICK_SIZEOF + 2 ];
	char          ee_AutoJoin[ SERVERENTRY_AUTOJOIN_SIZEOF + 2 ];

};


/*************************************************************************/

#endif /* MUICLASS_SERVERLIST_H */

