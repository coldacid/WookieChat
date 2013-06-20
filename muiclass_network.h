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
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>


#include "system.h"
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
MM_NETWORK_ADDLOG,
MM_NETWORK_LOGNOTECREATE,
MM_NETWORK_LOGNOTEADD,
MM_NETWORK_WAITSELECT,
MM_NETWORK_HANDLESOCKETS,

MM_NETWORK_SERVERFIND,
MM_NETWORK_SERVERALLOC,
MM_NETWORK_SERVERFREE,

MM_NETWORK_SERVERSOCKETINIT,
MM_NETWORK_SERVERSOCKETCLOSE,

MM_NETWORK_SERVERCONNECTAUTO,
MM_NETWORK_SERVERCONNECT,
MM_NETWORK_SERVERDISCONNECT,

MM_NETWORK_SERVERRECEIVEDATA,
MM_NETWORK_SERVERSENDDATA,

MM_NETWORK_CHANNELFIND,
MM_NETWORK_CHANNELALLOC,
MM_NETWORK_CHANNELFREE,

/* Attributes */
MA_NETWORK_OBJECTSETTINGS,
MA_NETWORK_OBJECTAUDIO,
};

struct MP_NETWORK_SERVERFIND           { ULONG MethodID; struct ServerEntry *ServerEntry; };
struct MP_NETWORK_SERVERALLOC          { ULONG MethodID; struct ServerEntry *ServerEntry; Object *WindowChat; };
struct MP_NETWORK_SERVERFREE           { ULONG MethodID; struct Server  *Server; };

struct MP_NETWORK_SERVERCONNECTAUTO    { ULONG MethodID; };
struct MP_NETWORK_SERVERSOCKETINIT     { ULONG MethodID; struct Server  *Server; };
struct MP_NETWORK_SERVERSOCKETCLOSE    { ULONG MethodID; struct Server  *Server; };

struct MP_NETWORK_SERVERCONNECT        { ULONG MethodID; struct Server  *Server; };
struct MP_NETWORK_SERVERDISCONNECT     { ULONG MethodID; struct Server  *Server; };

struct MP_NETWORK_SERVERRECEIVEDATA    { ULONG MethodID; struct Server  *Server; char *Data; ULONG Length; };
struct MP_NETWORK_SERVERSENDDATA       { ULONG MethodID; struct Server  *Server; char *Data; ULONG Length; };

struct MP_NETWORK_ADDLOG               { ULONG MethodID; struct Server  *Server; ULONG Type; char *format; IPTR arg1; IPTR arg2; IPTR arg3; IPTR arg4; };
struct MP_NETWORK_WAITSELECT           { ULONG MethodID; ULONG *SignalMask; };

struct MP_NETWORK_CHANNELFIND          { ULONG MethodID; struct Server  *Server; char *Name; };
struct MP_NETWORK_CHANNELALLOC         { ULONG MethodID; struct Server  *Server; char *Name; };
struct MP_NETWORK_CHANNELFREE          { ULONG MethodID; struct Server  *Server; struct Channel *Channel; };

/*************************************************************************/

struct Server {
	struct Server *s_Succ;
	struct Server *s_Pred;
/* copied over from serverentry */
	ULONG              s_Port;
	char               s_Name[ SERVERENTRY_NAME_SIZEOF   + 2 ];
	char               s_Address[ SERVERENTRY_ADDRESS_SIZEOF   + 2 ];
	char               s_Charset[ SERVERENTRY_CHARSET_SIZEOF   + 2 ];
	char               s_Password[ SERVERENTRY_PASSWORD_SIZEOF + 2 ];
	struct List        s_ChannelList;
	struct List        s_NickList;
/* network runtime data */
	ULONG              s_State; /* this is the current state */
	ULONG              s_Retry;
	//struct sockaddr_in s_ServerSocket;
	LONG               s_ServerSocket;
/* ident */
	long               s_IdentSocket;
	long               s_Ident_a_Socket;
	struct sockaddr_in s_IdentTest;
	long               ident_listen_socket;
	struct sockaddr_in ident_their_addr;
	char               ident_buffer[2000];

};

struct Channel {
	struct Channel    *c_Succ;
	struct Channel    *c_Pred;
	ULONG              c_Flags;
	char               c_Name[ CHANNELENTRY_CHANNEL_SIZEOF        + 2 ];
	char               c_Password[ CHANNELENTRY_PASSWORD_SIZEOF   + 2 ];
	Object            *c_ChatWindow;
	struct List        c_ChatLog;
};

#define CHANNELF_SERVER 1 /* this is the servers channel */

struct Nick {
	struct Nick       *n_Succ;
	struct Nick       *n_Pred;
	char               n_Name[ NICKENTRY_NICK_SIZEOF       + 2 ];
	char               n_Password[ NICKENTRY_NICK_SIZEOF   + 2 ];
};

enum{
SVRSTATE_NOTCONNECTED = 0,
SVRSTATE_CONNECTED,
SVRSTATE_RETRY,
SVRSTATE_FAILED,
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

