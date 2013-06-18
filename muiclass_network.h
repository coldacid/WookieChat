/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MUICLASS_NETWORK_H
#define MUICLASS_NETWORK_H 1

/*************************************************************************/

#include <exec/types.h>
#include "intern.h"
#include "muiclass.h"
#include "muiclass_serverlist.h"
#include "muiclass_channellist.h"
#include "muiclass_nicklist.h"

#define NetworkObject NEWOBJECT( appclasses[ CLASSID_NETWORK ]->mcc_Class, NULL

/*
** Methods and attributes
*/

enum {
MM_NETWORK_AUTORECONNECT =  0xFED00360,
MM_NETWORK_CONNECT,
MM_NETWORK_CONNECTALLOC,
MM_NETWORK_ADDLOG,
MM_NETWORK_LOGNOTECREATE,
MM_NETWORK_LOGNOTEADD,
MM_NETWORK_WAITSELECT,
MM_NETWORK_HANDLESOCKETS,
/* Attributes */
MA_NETWORK_OBJECTSETTINGS,
MA_NETWORK_OBJECTAUDIO,
};

struct MP_NETWORK_CONNECTALLOC  { ULONG MethodID; struct ServerEntry *ServerEntry; Object *Chat; };
struct MP_NETWORK_CONNECT       { ULONG MethodID; struct ServerEntry *ServerEntry; Object *Chat; };
struct MP_NETWORK_ADDLOG        { ULONG MethodID; struct Connection  *Connection; ULONG Type; char *format; IPTR arg1; IPTR arg2; IPTR arg3; IPTR arg4; };
struct MP_NETWORK_WAITSELECT    { ULONG MethodID; ULONG *SignalMask; };

/*************************************************************************/

struct Connection {
	struct Connection *c_Next;
	struct Connection *c_Pred;
/* copied over from serverentry */
	ULONG              c_Port;
	char               c_Name[ SERVERENTRY_NAME_SIZEOF   + 2 ];
	char               c_Address[ SERVERENTRY_ADDRESS_SIZEOF   + 2 ];
	char               c_Charset[ SERVERENTRY_CHARSET_SIZEOF   + 2 ];
	char               c_Password[ SERVERENTRY_PASSWORD_SIZEOF + 2 ];
	struct List        c_Channels;
/* network runtime data */
	ULONG              c_State; /* this is the current state */
	struct sockaddr_in c_ServerSocket;  /* geit FIXME: change name */

	long               c_a_socket;
/* ident */
	long               ident_a_socket;
	long               ident_listen_socket;
	struct sockaddr_in ident_test;
	struct sockaddr_in ident_their_addr;
	char               ident_buffer[2000];

};

struct Channel {
	char               c_Name[ CHANNELENTRY_CHANNEL_SIZEOF        + 2 ];
	char               c_Password[ CHANNELENTRY_PASSWORD_SIZEOF   + 2 ];
	Object            *c_ChatWindow;
};

enum{
SVRSTATE_NOTCONNECTED = 0,
SVRSTATE_CONNECTED,
SVRSTATE_RETRY,
SVRSTATR_FAILED,
};
enum{
LOGTYPE_ERROR = 0,
LOGTYPE_ACTION,
LOGTYPE_NORMAL,
LOGTYPE_NICKLISTTX,
LOGTYPE_TABSPEN,
LOGTYPE_JOIN,
LOGTYPE_PART,
LOGTYPE_QUIT,
LOGTYPE_MODES,
LOGTYPE_CTCP,
LOGTYPE_ACTIONS,
LOGTYPE_INFO,
LOGTYPE_OWNTEXT,
LOGTYPE_HIGHLIGHT,
LOGTYPE_NOTICES,
LOGTYPE_INVITE,
LOGTYPE_KICK,
LOGTYPE_NICKCHANGE,
LOGTYPE_TOPIC,
LOGTYPE_WALLOPS,
LOGTYPE_ACTIVITY,
LOGTYPE_CHATACTIVITY,
LOGTYPE_HIGHLIGHTACTIVITY,
LOGTYPE_LAST
};


/*
** Prototypes
*/

ULONG   MCC_Network_InitClass( void );
void    MCC_Network_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_NETWORK_H */

