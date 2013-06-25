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
#include <dos/dos.h>
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
MM_NETWORK_ =  0xFED00b00,

MM_NETWORK_SERVERFIND,
MM_NETWORK_SERVERALLOC,
MM_NETWORK_SERVERFREE,
MM_NETWORK_SERVERFINDCHANNEL,

MM_NETWORK_SERVERSOCKETINIT,
MM_NETWORK_SERVERSOCKETCLOSE,

MM_NETWORK_SERVERCONNECTAUTO,
MM_NETWORK_SERVERCONNECT,
MM_NETWORK_SERVERDISCONNECT,
MM_NETWORK_SERVERLOGIN,
MM_NETWORK_SERVERAUTOJOIN,

MM_NETWORK_SERVERMESSAGERECEIVED,
MM_NETWORK_SERVERMESSAGESEND,
MM_NETWORK_SERVERMESSAGESENDPROC,
MM_NETWORK_SERVERMESSAGEPROCESS,
MM_NETWORK_SERVERMESSAGEPARSEBEGIN,
MM_NETWORK_SERVERMESSAGEPARSEEND,
 
MM_NETWORK_SERVERRECEIVEDATA,
MM_NETWORK_SERVERSENDDATA,

MM_NETWORK_WAITSELECT,

MM_NETWORK_CHANNELFIND,
MM_NETWORK_CHANNELALLOC,
MM_NETWORK_CHANNELFREE,

MM_NETWORK_CHATLOGENTRYFREE,
MM_NETWORK_CHATLOGENTRYADD,

MM_NETWORK_CHATNICKENTRYALLOC,
MM_NETWORK_CHATNICKENTRYFREE,

/* Attributes */
MA_NETWORK_OBJECTSETTINGS,
MA_NETWORK_OBJECTAUDIO,
};

struct MP_NETWORK_SERVERFIND              { ULONG MethodID; struct ServerEntry *ServerEntry; };
struct MP_NETWORK_SERVERALLOC             { ULONG MethodID; struct ServerEntry *ServerEntry; };
struct MP_NETWORK_SERVERFREE              { ULONG MethodID; struct Server *Server; };
struct MP_NETWORK_SERVERFINDCHANNEL       { ULONG MethodID; struct Server *Server; char *ChannelName; };

struct MP_NETWORK_SERVERCONNECTAUTO       { ULONG MethodID; };
struct MP_NETWORK_SERVERSOCKETINIT        { ULONG MethodID; struct Server *Server; };
struct MP_NETWORK_SERVERSOCKETCLOSE       { ULONG MethodID; struct Server *Server; };

struct MP_NETWORK_SERVERCONNECT           { ULONG MethodID; struct Server *Server; };
struct MP_NETWORK_SERVERDISCONNECT        { ULONG MethodID; struct Server *Server; };

struct MP_NETWORK_SERVERLOGIN             { ULONG MethodID; struct Server *Server; };
struct MP_NETWORK_SERVERAUTOJOIN          { ULONG MethodID; struct Server *Server; };

struct MP_NETWORK_SERVERMESSAGERECEIVED   { ULONG MethodID; struct Server *Server; char *Message; };
struct MP_NETWORK_SERVERMESSAGESEND       { ULONG MethodID; struct Server *Server; char *Message; };
struct MP_NETWORK_SERVERMESSAGEPARSEBEGIN { ULONG MethodID; struct Server *Server; char *Message; };
struct MP_NETWORK_SERVERMESSAGEPARSEEND   { ULONG MethodID; struct ServerMessageParse *ServerMessageParse; };
struct MP_NETWORK_SERVERMESSAGEPROCESS    { ULONG MethodID; struct Server *Server; struct ServerMessageParse *ServerMessageParse; };

struct MP_NETWORK_SERVERRECEIVEDATA       { ULONG MethodID; struct Server *Server; char *Data; ULONG Length; };
struct MP_NETWORK_SERVERSENDDATA          { ULONG MethodID; struct Server *Server; char *Data; ULONG Length; };

struct MP_NETWORK_WAITSELECT              { ULONG MethodID; ULONG *SignalMask; };

struct MP_NETWORK_CHANNELFIND             { ULONG MethodID; struct Server *Server; char *Name; };
struct MP_NETWORK_CHANNELALLOC            { ULONG MethodID; struct Server *Server; char *Name; };
struct MP_NETWORK_CHANNELFREE             { ULONG MethodID; struct Server *Server; struct Channel *Channel; };

struct MP_NETWORK_CHATLOGENTRYADD         { ULONG MethodID; struct Server *Server; struct Channel *Channel; ULONG Pen; char *Message; };
struct MP_NETWORK_CHATLOGENTRYFREE        { ULONG MethodID; struct ChatLogEntry *ChatLogEntry; };

struct MP_NETWORK_CHATNICKENTRYALLOC      { ULONG MethodID; struct Channel *Channel; char *NickName; };
struct MP_NETWORK_CHATNICKENTRYFREE       { ULONG MethodID; struct Channel *Channel; struct ChatNickEntry *ChatNickEntry; };

/*************************************************************************/

#define SERVERBUFFER_SIZEOF 0xffff

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
	struct List        s_SendList;
/* network runtime data */
	ULONG              s_Flags;
	ULONG              s_State; /* this is the current state */
	ULONG              s_Retries;
	char               s_Buffer[ SERVERBUFFER_SIZEOF + 1 ];
	LONG 			   s_BufferFilled;
	//struct sockaddr_in s_ServerSocket;
	LONG               s_ServerSocket;
/* ident */
	long               s_IdentSocket;
	long               s_Ident_a_Socket;
	struct sockaddr_in s_IdentTest;
//	  long               ident_listen_socket;
//	  struct sockaddr_in ident_their_addr;
//	  char               ident_buffer[2000];

};


struct Channel {
	struct Channel    *c_Succ;
	struct Channel    *c_Pred;
	struct List        c_ChatLogList;
	struct List        c_ChatNickList;
	char              *c_Topic;
	ULONG              c_Flags;
	char               c_Name[ CHANNELENTRY_CHANNEL_SIZEOF        + 2 ];
	char               c_Password[ CHANNELENTRY_PASSWORD_SIZEOF   + 2 ];
};

#define CHANNELF_SERVER          1     /* this is the servers channel */
#define CHANNELF_NAMESLIST       2  /* set after first NAMES command and cleared on last */
#define CHANNELF_MESSAGEOFTHEDAY 4  /* set after MOTD START arrives and clears on MOTD END */

struct Nick {
	struct Nick       *n_Succ;
	struct Nick       *n_Pred;
	char               n_Name[ NICKENTRY_NICK_SIZEOF       + 2 ];
	char               n_Password[ NICKENTRY_NICK_SIZEOF   + 2 ];
};

struct ChatLogEntry {
	struct ChatLogEntry *cle_Succ;
	struct ChatLogEntry *cle_Pred;
	ULONG                cle_Pen;
	char                 cle_PreParse[16]; /* used by display method */
	char                 cle_Message[1];
};


struct ChatNickEntry {
	struct ChatNickEntry *cne_Succ;
	struct ChatNickEntry *cne_Pred;
	char                  cne_Nick[1];
};
enum{
SVRSTATE_NOTCONNECTED = 0,
SVRSTATE_PRECONNECTED,
SVRSTATE_CONNECTED,
SVRSTATE_RETRY,
SVRSTATE_FAILED,
};


/*
** Prototypes
*/

ULONG   MCC_Network_InitClass( void );
void    MCC_Network_DisposeClass( void );

/*************************************************************************/

#endif /* MUICLASS_NETWORK_H */

