/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_network.c
*/

#define NODEBUG

#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/socket.h>

#include <libraries/mui.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "functions.h"
#include "irccommand.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowchat.h"
#include "muiclass_network.h"
#include "muiclass_chatlog.h"
#include "muiclass_settingscolor.h"
#include "version.h"

/*************************************************************************/

enum{
WID_SETTINGS = 0,
GID_AUDIO,
GID_LAST
};


/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	struct List            mcc_ServerList;
	fd_set                 mcc_ReadMaster;       // master file descriptor list
	fd_set                 mcc_WriteMaster;      // master file descriptor list
	int                    mcc_FDMax;            // maximum file descriptor number
	fd_set                 mcc_ReadFDS;          // master file descriptor list
	fd_set                 mcc_WriteFDS;         // master file descriptor list
};

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

#define COMMAND_COMPOSEBUFFER_SIZEOF 0x1000

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = (Object*) DoSuperNew( cl, obj, TAG_DONE ) ) ) {
		struct mccdata *mccdata = INST_DATA( cl, obj );
		
		SetAttrs( obj, TAG_MORE, msg->ops_AttrList );

		NEWLIST( &mccdata->mcc_ServerList ); /* init list */

		/* init defaults */
		mccdata->mcc_FDMax = -1;

	}
	return( (ULONG) obj );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	while( (s = (APTR) mccdata->mcc_ServerList.lh_Head)->s_Succ ) {
		DoMethod( obj, MM_NETWORK_SERVERFREE, s );
	}

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Set()
**
*/

/*************************************************************************/

static ULONG OM_Set( struct IClass *cl, Object *obj, struct opSet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct TagItem *tag;
struct TagItem *tstate;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_NETWORK_OBJECTSETTINGS:
				mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (APTR) tidata;
				break;
			case MA_NETWORK_OBJECTAUDIO:
				mccdata->mcc_ClassObjects[ GID_AUDIO    ] = (APTR) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_ServerConnectAuto()
**
*/

/*************************************************************************/

static ULONG MM_ServerConnectAuto( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERCONNECTAUTO *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se;
struct Server      *s;
Object *serverlist;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	serverlist = (APTR) LocalReadConfig( OID_SVR_LIST );

	for( i = 0 ; ; i++ ) {
		se = NULL;
		DoMethod( serverlist, MUIM_NList_GetEntry, i, &se );
		if( se ) {
			if( ( se->se_Flags & SERVERENTRYF_AUTOCONNECT ) ) {
				if( ( s = (APTR) DoMethod( obj, MM_NETWORK_SERVERALLOC, se ) ) ) {
					if( ( s->s_State != SVRSTATE_CONNECTED ) ) {
						DoMethod( obj, MM_NETWORK_SERVERCONNECT , s );
						DoMethod( obj, MM_NETWORK_SERVERLOGIN   , s );
						DoMethod( obj, MM_NETWORK_SERVERAUTOJOIN, s );
					}
				}
			}
		} else {
			break;
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerLogin()
**
*/

/*************************************************************************/

static ULONG MM_ServerLogin( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERLOGIN *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s = msg->Server;
struct Nick   *n;
char *buffer;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( buffer = AllocVec( COMMAND_COMPOSEBUFFER_SIZEOF, MEMF_ANY ) ) ) {

		n = (APTR) s->s_NickList.lh_Head;
		if( n->n_Succ ) {
			sprintf( buffer, "/NICK %s", n->n_Name );
			strcpy( s->s_CurrentNick, n->n_Name );
		} else {
			sprintf( buffer, "/NICK User_%ld", &i );
			sprintf( s->s_CurrentNick, "User_%ld", &i );
		}
		DoMethod( obj, MM_NETWORK_SERVERMESSAGESENDMSG, s, NULL, buffer );

		/* user */
		sprintf( buffer, "/USER %s 8 * : " APPLICATIONNAME, LRC(OID_SVR_USERNAME) );
		DoMethod( obj, MM_NETWORK_SERVERMESSAGESENDMSG, s, NULL, buffer );

		FreeVec( buffer );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerAutoJoin()
**
*/

/*************************************************************************/

static ULONG MM_ServerAutoJoin( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERAUTOJOIN *msg )
{
struct Server  *s = msg->Server;
struct Channel *c;
char *buffer;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( buffer = AllocVec( COMMAND_COMPOSEBUFFER_SIZEOF, MEMF_ANY ) ) ) {
		for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; c = c->c_Succ ) {
			if( !( c->c_Flags & CHANNELF_SERVER ) ) { /* server tab is no real channel */
				sprintf( buffer, "/JOIN %s", c->c_Name );
				if( c->c_Password[0] ) {
					strcat( buffer, " " );
					strcat( buffer, c->c_Password );
				}
				DoMethod( obj, MM_NETWORK_SERVERMESSAGESENDMSG, s, NULL, buffer );
			}
		}
		FreeVec( buffer );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerFind()
**
*/

/*************************************************************************/

static ULONG MM_ServerFind( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERFIND *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se = msg->ServerEntry;
struct Server *s;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( s = (APTR) mccdata->mcc_ServerList.lh_Head ; s->s_Succ ; s = s->s_Succ ) {
		if( s->s_Port == se->se_Port ) { /* different port -> different server */
			if( !Stricmp( (CONST_STRPTR) s->s_Address, (CONST_STRPTR) se->se_Address ) ) {
				return( (IPTR) s );
			}
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerAlloc()
**
*/

/*************************************************************************/

static ULONG MM_ServerAlloc( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERALLOC *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server       *s = NULL;
struct ServerEntry  *se;
struct Nick         *n;
struct NickEntry    *ne;
struct Channel      *c;
struct ChannelEntry *ce;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( se = msg->ServerEntry ) ) {
		/* do not alloc the same server twice */
		if( !( s = (struct Server *) DoMethod( obj, MM_NETWORK_SERVERFIND, se ) ) ) {
			if( ( s = AllocVec( sizeof( struct Server ), MEMF_ANY|MEMF_CLEAR ) ) ) {
				NEWLIST( &s->s_ChannelList );
				NEWLIST( &s->s_NickList );
				NEWLIST( &s->s_SendList );

				AddTail( &mccdata->mcc_ServerList, (struct Node *) s );

				s->s_ServerSocket      = -1;
				s->s_Ident_a_Socket    = -1;
				s->s_IdentSocket       = -1;

				/* we cannot link to server entry as it may be removed by user during
				** runtime, so we spy important data */
				strcpy( s->s_Name    , se->se_Name     );
				strcpy( s->s_Address , se->se_Address  );
				strcpy( s->s_Charset , se->se_Charset  );
				strcpy( s->s_Password, se->se_Password );

				if( !( s->s_Port = se->se_Port ) ) {
					s->s_Port = 6667;
				}
				s->s_State = SVRSTATE_NOTCONNECTED;

				/* add a server channel */
				if( ( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELALLOC, s, s->s_Name ) ) ) {
					c->c_Flags      |= CHANNELF_SERVER;
					DoMethod( _app(obj), MM_APPLICATION_CHANNELADD, c );
				}

				for( ce = (APTR) se->se_ChannelList.lh_Head ; ce->ce_Succ ; ce = ce->ce_Succ ) {
					if( ( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELALLOC, s, ce->ce_Name ) ) ) {
						strcpy( c->c_Password, ce->ce_Password );
					}
				}
				for( ne = (APTR) se->se_NickList.lh_Head ; ne->ne_Succ ; ne = ne->ne_Succ ) {
					if( ( n = AllocVec( sizeof( struct Channel ), MEMF_ANY ) ) ) {
						AddTail( &s->s_NickList, (struct Node *) n );
						strcpy( n->n_Name    , ne->ne_Name     );
						strcpy( n->n_Password, ne->ne_Password );
					}
				}
			}
		}
	}
	return( (IPTR) s );
}
/* \\\ */
/* /// MM_ServerFree()
**
*/

/*************************************************************************/

static ULONG MM_ServerFree( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERFREE *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server   *s = msg->Server;
struct Nick         *n;
struct Channel      *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* dissnnect if needed */

	DoMethod( obj, MM_NETWORK_SERVERDISCONNECT, s );

	/* remove all channels */
	while( ( c = (APTR) s->s_ChannelList.lh_Head )->c_Succ ) {
		DoMethod( obj, MM_NETWORK_CHANNELFREE, s, c );
	}

	/* remove all nicks */
	while( ( n = (APTR) s->s_NickList.lh_Head )->n_Succ ) {
		/* remove from list */
		Remove( (struct Node *) n );
		/* free structure */
		FreeVec( n );
	}
	while( ( n = (APTR) s->s_SendList.lh_Head )->n_Succ ) {
		Remove( (struct Node *) n );
		FreeVec( n );
	}
	/* remove from list */
	Remove( (struct Node *) s );
	/* free structure */
	FreeVec( s );

	return( 0 );
}
/* \\\ */

/* /// MM_ServerSocketInit()
**
*/

static ULONG MM_ServerSocketInit( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERSOCKETINIT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s;
ULONG result;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	result = MSG_ERROR_NOERROR + 1; /* socket error */
	if( ( s = msg->Server ) ) {

		DoMethod( obj, MM_NETWORK_SERVERSOCKETCLOSE, s );

		if( ( s->s_ServerSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) != INVALID_SOCKET ) {
			ULONG on = 1;
			result = MSG_ERROR_NOERROR + 2; /* invalid socket */

			if( setsockopt( s->s_ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char const*) &on, sizeof( ULONG ) ) != -1 ) {
				if( mccdata->mcc_FDMax < s->s_ServerSocket ) {
					mccdata->mcc_FDMax = s->s_ServerSocket;
				}

				if ( s->s_ServerSocket != -1 ) {
					FD_SET( s->s_ServerSocket, &mccdata->mcc_ReadMaster  );
					FD_CLR( s->s_ServerSocket, &mccdata->mcc_WriteMaster );
				}


				if( 0 ) {  /* add ident config here */
					s->s_Ident_a_Socket = -1;
					if( ( s->s_IdentSocket = socket( AF_INET, SOCK_STREAM, 0 ) ) != INVALID_SOCKET ) {
						if( mccdata->mcc_FDMax < s->s_IdentSocket ) {
							mccdata->mcc_FDMax = s->s_IdentSocket;
						}
					}

					memset( &s->s_IdentTest, '\0', sizeof( s->s_IdentTest) );
					s->s_IdentTest.sin_family      = AF_INET; // host byte order
					s->s_IdentTest.sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
					s->s_IdentTest.sin_port        = htons(113); // short, network byte order

					ULONG yes = 1;

					if( setsockopt( (int) s->s_IdentSocket, (int) SOL_SOCKET, (int) SO_REUSEADDR, &yes, (int) sizeof( yes ) ) == -1 ) {
						debug( "unable to reuse address\n" );
					}
					if( setsockopt( (int) s->s_IdentSocket, (int) SOL_SOCKET, (int) SO_REUSEPORT, &yes, (int) sizeof( yes ) ) == -1 ) {
						debug( "unable to reuse port\n" );
					}
					if( bind( s->s_IdentSocket, (struct sockaddr *) &s->s_IdentTest, sizeof( s->s_IdentTest) ) == -1 ) {
						debug( "unable to bind address to socket, error number: %ld\n", Errno() );
					}
					if( IoctlSocket( s->s_IdentSocket, FIONBIO, (char*) &yes ) < 0 ) {
						debug("unable to change non-blocking I/O option for socket, error number:%ld\n", Errno() );
					}
					FD_SET( s->s_IdentSocket, &mccdata->mcc_ReadMaster );

					listen( (int) s->s_IdentSocket, (int) 5 );
				}
				if( IoctlSocket( s->s_ServerSocket, FIONBIO, (char*) &on) >= 0 ) {
					result = MSG_ERROR_NOERROR; /* invalid socket */
				}
			}
		}
    }
	debug("MM_ServerSocketInit() - Done %08lx\n", result );
	return( result );
}
/* \\\ */
/* /// MM_ServerSocketClose()
**
*/

static ULONG MM_ServerSocketClose( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERSOCKETCLOSE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( s = msg->Server ) ) {
		if( s->s_ServerSocket != -1 ) { /* still open? */
			FD_CLR( s->s_ServerSocket, &mccdata->mcc_ReadMaster  );
			FD_CLR( s->s_ServerSocket, &mccdata->mcc_WriteMaster );

			CloseSocket( s->s_ServerSocket );
			s->s_ServerSocket = -1;
		}
		if( s->s_IdentSocket != -1 ) { /* still open? */
			CloseSocket( s->s_IdentSocket );
			s->s_IdentSocket = -1;
		}
		s->s_State        = SVRSTATE_NOTCONNECTED;
		s->s_BufferFilled = 0;
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ServerConnect()
**
*/

/*************************************************************************/

static ULONG MM_ServerConnect( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERCONNECT *msg )
{
struct Server *s = msg->Server;
ULONG result;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	result = MSG_ERROR_NOERROR + 4; /* bsdsocket missing */
	if( !SocketBase ) {
		SocketBase = OpenLibrary( (_ub_cs) "bsdsocket.library", 0L );
#ifdef __amigaos4__
		ISocket = (struct SocketIFace*) GetInterface( (struct Library *) SocketBase, "main", 1, NULL );
#endif
    }

	if( SocketBase ) {
		struct hostent *he;
		result = MSG_ERROR_NOERROR + 5; /* host error */

		if( !( result = DoMethod( obj, MM_NETWORK_SERVERSOCKETINIT, s ) ) ) {
//			  debug("connecting to '%s' Port %ld\n",s->s_Address, s->s_Port );
			if( ( he = gethostbyname( (const UBYTE *) s->s_Address) ) ) {
				struct sockaddr_in addr;

				addr.sin_family = AF_INET;
				addr.sin_port   = htons( s->s_Port );
				addr.sin_addr   = *((struct in_addr *) he->h_addr);;

				memset( &( addr.sin_zero ), '\0', 8 );

				result = MSG_ERROR_NOERROR + 6; /* socket error */
				connect( s->s_ServerSocket, (struct sockaddr*) &addr, sizeof( struct sockaddr ) );
				s->s_State = SVRSTATE_CONNECTED;

				DoMethod( obj, MM_NETWORK_SERVERMESSAGESENDMSG, s, NULL, "/HELLO" );

				result = MSG_ERROR_NOERROR;
				return( result );

			}
			if( result )  {
				DoMethod( obj, MM_NETWORK_SERVERSOCKETCLOSE, s );
				s->s_Retries++;
				if( s->s_Retries <= 5 ) {
					s->s_State = SVRSTATE_RETRY;
				} else {
					s->s_State = SVRSTATE_FAILED;
				}
			}
		}
    }
//	  debug("MM_ServerConnect() - Done %08lx\n", result );
	return( result );
}
/* \\\ */
/* /// MM_ServerDisconnect()
**
*/

/*************************************************************************/

static ULONG MM_ServerDisconnect( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERDISCONNECT *msg )
{
struct Server *s = msg->Server;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( SocketBase ) {
		DoMethod( obj, MM_NETWORK_SERVERSOCKETCLOSE, s );
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ServerMessageReceived()
**
*/

static ULONG MM_ServerMessageReceived( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERMESSAGERECEIVED *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server  *s = msg->Server;
struct ServerMessageParse *smp;
struct ChatLogEntry *cle;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( smp = (APTR) DoMethod( obj, MM_NETWORK_SERVERMESSAGEPARSEBEGIN, s, msg->Message ) ) ) {

		/* now we deal with the data. handle ping, user lists and other stuff */

		if( !( DoMethod( obj, MM_NETWORK_SERVERMESSAGEPROCESS, s, smp ) & IRCCMD_RESULTF_IGNOREMESSAGE ) ) {
		
			/* insert URL grabber stuff here */

			/* finaly distribute message to all chat windows hosting the related channel */
			if( ( cle = (APTR) DoMethod( obj, MM_NETWORK_CHATLOGENTRYADD, s, smp->smp_Channel, smp->smp_Pen, smp->smp_MessageBuffer ) ) ) {
				DoMethod( _app(obj), MM_APPLICATION_MESSAGERECEIVED, cle );
			}
		}
		DoMethod( obj, MM_NETWORK_SERVERMESSAGEPARSEEND, smp );
	}
	return( 0 );
}
/* \\\ */

struct SendNode {
	struct SendNode *sn_Succ;
	struct SendNode *sn_Pred;
	char             sn_Message[0];
};

/* /// MM_ServerMessageSendMsg()
**
*/

/*************************************************************************/

static ULONG MM_ServerMessageSendMsg( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERMESSAGESENDMSG *msg )
{
struct SendNode *sn;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( sn = AllocVec( sizeof( struct SendNode ) + strlen( msg->Message ) + strlen( "PRIVMSG :" ) + strlen( msg->Channel->c_Name ) + 64, MEMF_ANY|MEMF_CLEAR ) )) {
		if( *msg->Message != '/' ) {
			strcpy( sn->sn_Message, (char *) "PRIVMSG " );
			strcat( sn->sn_Message, msg->Channel->c_Name );
			strcat( sn->sn_Message, (char *) " :" );
			strcat( sn->sn_Message, (char *) msg->Message );
			strcat( sn->sn_Message, "\r\n" );
			/* local echo */
			DoMethod( obj, MM_NETWORK_SERVERMESSAGERECEIVED, msg->Server, sn->sn_Message );
		} else {
			strcpy( sn->sn_Message, (char *) &msg->Message[1] );
			strcat( sn->sn_Message, "\r\n" );
		}
		AddTail( &msg->Server->s_SendList, (struct Node *) sn );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerMessageSendProc()
**
*/

/*************************************************************************/

static ULONG MM_ServerMessageSendProc( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server   *s;
struct SendNode *sn;

//	  debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( s = (APTR) mccdata->mcc_ServerList.lh_Head ; s->s_Succ ; s = s->s_Succ ) {
		if( ( sn = (APTR) s->s_SendList.lh_Head )->sn_Succ ) {
			DoMethod( obj, MM_NETWORK_SERVERSENDDATA, s, sn->sn_Message, 0 );
			Remove( (struct Node *) sn );
			FreeVec( sn );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerMessageParseBegin()
**
*/

/*************************************************************************/

/* /// SMP_SplitNickUserHost()
*/

/*************************************************************************/

static char *SMP_SplitNickUserHost( struct ServerMessageParse *smp, char *src, char *dst )
{
char *tmp;

	if( *src ) { /* no data, no split */
		strcpy( dst, src );
		if( ( tmp = strchr( dst, '!' ) ) ) {
			smp->smp_FromNick = dst;
			*tmp++ = 0x00; /* terminate */
			dst = tmp; /* skip string */
			if( *tmp ) {
				smp->smp_FromUserID = dst;
				if( ( tmp = strchr( dst, '@' ) ) ) {
					*tmp++ = 0x00; /* terminate */
					dst = tmp; /* skip string */
					if( *tmp ) {
						smp->smp_FromHost = dst;
						dst = &tmp[ strlen( tmp ) + 1 ];
					}
				} else {
					dst = &tmp[ strlen( tmp ) + 1 ];
				}
			}
		} else {
			smp->smp_FromHost = smp->smp_From;
		}
	}
	return( dst );
}
/* \\\ */

static ULONG MM_ServerMessageParseBegin( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERMESSAGEPARSEBEGIN *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerMessageParse *smp;
char *args, chr, *pattern, *dst, *tmp;
ULONG i;

//	  debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( smp = AllocVec( sizeof( struct ServerMessageParse ) + strlen( msg->Message ) + 1, MEMF_ANY|MEMF_CLEAR ) ) ) {
		smp->smp_Pen = PEN_LOGPRIVMSG;
		strcpy( smp->smp_Data, msg->Message );
		dst = smp->smp_StringStorage;
		strcpy( dst, msg->Message );
		dst = &dst[ strlen( dst ) + 1 ];

		/* dump to shell */
		VPrintf( (CONST_STRPTR) msg->Message, (CONST APTR) smp );
		VPrintf( (CONST_STRPTR) "\n", (CONST APTR) smp );
//		  debug("%128lh\n", msg->Message );
		DateStamp( &smp->smp_DateStamp );

		smp->smp_MessageBuffer[0] = 0x00; /* terminate message string */
		if( ( GlobalReadConfig( OID_GUI_TIMESHOW ) ) ) {
			Locale_FormatDate( (char *) LocalReadConfig( OID_GUI_TIMEFORMAT ), smp->smp_MessageBuffer, &smp->smp_DateStamp, SMP_MESSAGEBUFFER_SIZEOF );
			strcat( smp->smp_MessageBuffer, " " );
		}

		smp->smp_From       = STR_NUL;
//		  smp->smp_Command    = STR_NUL;
//		  smp->smp_Arguments  = STR_NUL;
//		  smp->smp_Message    = STR_NUL;
		smp->smp_Channel    = STR_NUL;
		smp->smp_Nick       = STR_NUL;
		smp->smp_FromNick   = STR_NUL;
		smp->smp_FromUserID = STR_NUL;
		smp->smp_FromHost   = STR_NUL;
		smp->smp_Flags      = STR_NUL;
		smp->smp_Date       = STR_NUL;

	/* irc message lines look like this:
	** ":<from> <command> <params> :<message>"
	**
	*/

		if( ( smp->smp_Message = strstr( smp->smp_Data, " :" ) ) ) {
			smp->smp_Message[0] = 0x00;
			smp->smp_Message += 2;
		} else {
			smp->smp_Message = STR_NUL;
		}

		smp->smp_Command = smp->smp_Data;
		if( ( smp->smp_Data[0] == ':' ) ) {
			smp->smp_From = smp->smp_Data;
			smp->smp_From++;
			if( ( smp->smp_Command = strchr( smp->smp_Data, 0x20 ) ) ) {
				*smp->smp_Command++ = 0x00;
			}
		}

		if( ( smp->smp_Arguments = strchr( smp->smp_Command, 0x20 ) ) ) {
			*smp->smp_Arguments++ = 0x00;
		} else {
			smp->smp_Arguments = STR_NUL;
		}
	/* split the from argument in nick, userid, host if possible */
		dst = SMP_SplitNickUserHost( smp, smp->smp_From, dst );

		for( i = 0 ; TAB_IRCCOMMANDS[ i ].CMD_Name ; i++ ) {
			if( !Stricmp( (CONST_STRPTR) TAB_IRCCOMMANDS[ i ].CMD_Name, (CONST_STRPTR) smp->smp_Command ) ) {
				pattern	= TAB_IRCCOMMANDS[ i ].CMD_ArgPattern;
				args    = smp->smp_Arguments;
				if( pattern && pattern[0] && args && args[0] ) {
//					  debug("processing '%s' -> '%s'\n", smp->smp_Command, TAB_IRCCOMMANDS[ i ].CMD_ArgPattern );

					while( args && *args && ( chr = *pattern++ ) ) {
						if( ( tmp = strchr( args, 0x20 ) ) ) {
							*tmp++ = 0x00;
						}

						switch( chr ) {
							case 'C': /* channel */
								smp->smp_Channel = args;
								break;
							case '-': /* ignore  */
								break;
							case 'N': /* nick    */
								smp->smp_Nick = args;
								break;
							case '+': /* flags   */
								smp->smp_Flags = args;
								break;
							case 'F': /* geit!~fuhbar@geit.de  */
								dst = SMP_SplitNickUserHost( smp, args, dst );
								break;
							case 'D': /* 3412341244  */
								{
								time_t rawtime;
								struct tm *timeinfo;
								StrToLong( (CONST_STRPTR) args, &rawtime );
								timeinfo = localtime(&rawtime);
								smp->smp_Date = dst;
								strcpy( dst, asctime(timeinfo) );
								dst = &dst[ strlen( dst ) + 1 ];
								}
						}
						args = tmp;
					}
				} else {
//					  debug("has no argument pattern defined\n");
				}
				break;
			}
		}
		if( !TAB_IRCCOMMANDS[ i ].CMD_Name ) {
			VPrintf( (CONST_STRPTR) "!! Command is not in command table !!\n", &smp->smp_Command );
		}
	}
#if 1
	debug("# From:      '%s'\n", smp->smp_From );
	debug("# Command:   '%s'\n", smp->smp_Command );
	debug("# Args:      '%s'\n", smp->smp_Arguments );
	debug("# Message:   '%s'\n", smp->smp_Message );

	debug("# Channel:   '%s'\n", smp->smp_Channel );
	debug("# Nick:      '%s'\n", smp->smp_Nick  );
	debug("# Flags:     '%s'\n", smp->smp_Flags );
	debug("# FromNick:  '%s'\n", smp->smp_FromNick  );
	debug("# FromUserID:'%s'\n", smp->smp_FromUserID );
	debug("# FromHost:  '%s'\n\n----------------------------\n", smp->smp_FromHost  );
#endif
	return( (IPTR) smp );
}
/* \\\ */
/* /// MM_ServerMessageParseEnd()
**
*/

/*************************************************************************/

static ULONG MM_ServerMessageParseEnd( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERMESSAGEPARSEEND *msg )
{
//	  debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	FreeVec( msg->ServerMessageParse );

	return( 0 );
}
/* \\\ */
/* /// MM_ServerMessageProcess()
**
*/

/*************************************************************************/

static ULONG MM_ServerMessageProcess( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERMESSAGEPROCESS *msg )
{
struct ServerMessageParse *smp = msg->ServerMessageParse;
ULONG result = 0, i;

//	  debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( smp->smp_Command && smp->smp_Command[0] ) {
		for( i = 0 ; TAB_IRCCOMMANDS[i].CMD_Name ; i++ ) {
			if( !Stricmp( (CONST_STRPTR) smp->smp_Command, (CONST_STRPTR) TAB_IRCCOMMANDS[i].CMD_Name ) ) {
				result = TAB_IRCCOMMANDS[i].CMD_Function( obj, msg->Server, smp );
				break;
			}
		}
		/* commando was not found, so call fallback */
		if( !TAB_IRCCOMMANDS[i].CMD_Name ) {
			IRCCMD_GenericServer( obj, msg->Server, smp );
		}
	}
	return( result );
}
/* \\\ */

/* /// MM_ServerSendData()
**
*/

/*************************************************************************/

static ULONG MM_ServerSendData( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERSENDDATA *msg )
{
struct Server *s = msg->Server;
ULONG result;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	result = MSG_ERROR_NOERROR + 10;

	if( s->s_State == SVRSTATE_CONNECTED ) {
		if( ( send( s->s_ServerSocket, (CONST UBYTE *) msg->Data, ( msg->Length ? msg->Length : strlen( msg->Data ) ), 0 ) != -1 ) ) {
			result = MSG_ERROR_NOERROR;
		}
	}
	return( result );
}
/* \\\ */
/* /// MM_ServerReceiveData()
**
*/

/*************************************************************************/

static ULONG MM_ServerReceiveData( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERRECEIVEDATA *msg )
{
struct Server *s = msg->Server;
LONG bytes;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( s->s_State == SVRSTATE_CONNECTED ) {

		memset( msg->Data, 0, msg->Length );

		bytes = recv( s->s_ServerSocket, (UBYTE *) msg->Data, msg->Length - 1, 0 );

		if( bytes > 0 ) {
			return( bytes );
		} else {
			DoMethod( obj, MM_NETWORK_SERVERDISCONNECT );
		}
	}
	return(0);
}
/* \\\ */

/* /// MM_WaitSelect()
**
*/

/*************************************************************************/

static ULONG MM_WaitSelect( struct IClass *cl, Object *obj, struct MP_NETWORK_WAITSELECT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG selectresult;
LONG waitsignals = *((ULONG*) msg->SignalMask );

	mccdata->mcc_ReadFDS  = mccdata->mcc_ReadMaster;
	mccdata->mcc_WriteFDS = mccdata->mcc_WriteMaster;

	debug("%08lx %08lx\n", msg->SignalMask, *msg->SignalMask );
	selectresult  = WaitSelect( mccdata->mcc_FDMax + 1, &mccdata->mcc_ReadFDS, NULL, NULL, NULL, msg->SignalMask );

	if( selectresult > 0 ) {
		struct Server *s;
		LONG fd;

		for( fd = 0; fd <= mccdata->mcc_FDMax ; fd++ ) {
			if( FD_ISSET( fd, &mccdata->mcc_ReadFDS ) ) {
				for( s = (APTR) mccdata->mcc_ServerList.lh_Head ; s->s_Succ ; s = s->s_Succ ) {
					if( ( fd == s->s_ServerSocket ) || ( fd == s->s_Ident_a_Socket ) ) {
						break;
					}
				}
				if( s->s_Succ ) {
					LONG length;
					char *tmp;

					length  = recv( s->s_ServerSocket, (UBYTE*) &s->s_Buffer[ s->s_BufferFilled ], ( SERVERBUFFER_SIZEOF - s->s_BufferFilled ) , 0 );

					if( length > 0 ) {
						s->s_BufferFilled += length;
						s->s_Buffer[ s->s_BufferFilled ] = 0x00;
					}

					//if( s->s_BufferFilled ) {
					while( s->s_BufferFilled && ( tmp = strchr( s->s_Buffer, 0x0d ) ) ) {
						*tmp++ = 0x00;
						DoMethod( obj, MM_NETWORK_SERVERMESSAGERECEIVED, s, s->s_Buffer );
/* dump line */
						if( strlen( s->s_Buffer ) > 80 ) {
							s->s_Buffer[80] = 0x00;
						}

						while( ( *tmp == 0x0a || *tmp == 0x0d ) ) { tmp++; }
						s->s_BufferFilled -= ( ((ULONG) tmp) - ((ULONG) s->s_Buffer) );
						//debug("buffer fill %ld\n", s->s_BufferFilled );
						if( s->s_BufferFilled < 0 ) {
						//	  debug("WARN buffer fill %ld\n", s->s_BufferFilled );
							s->s_BufferFilled = 0;
						}

						if( s->s_BufferFilled ) {
							CopyMem( tmp, s->s_Buffer, s->s_BufferFilled );
						}
					}
					//}
					
					if( length <= 0 ) {
						if(FD_ISSET( s->s_ServerSocket, &mccdata->mcc_ReadMaster ) ) {
							FD_CLR( s->s_ServerSocket, &mccdata->mcc_ReadMaster );
						}
					}

					//if( s->s_State == SVRSTATE_PRECONNECTED ) {
				}
			}
//			  if( FD_ISSET( fd, &mccdata->mcc_WriteFDS ) ) {
//				  if( FD_ISSET( fd, &mccdata->mcc_WriteMaster ) ) {
//					  //FD_CLR( fd, &mccdata->mcc_WriteMaster);
//				  }
//			  }
		}

	} else {
		debug("wait select failed -> using normal wait to keep application alive\n");
		*((ULONG*) msg->SignalMask ) = Wait( waitsignals );
	}
	return( selectresult );
}
/* \\\ */

/* /// MM_ChannelFind()
**
*/

/*************************************************************************/

static ULONG MM_ChannelFind( struct IClass *cl, Object *obj, struct MP_NETWORK_CHANNELFIND *msg )
{
struct Server   *s = msg->Server;
struct Channel      *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; c = c->c_Succ ) {
		if( msg->Name ) {
			if( !Stricmp( (CONST_STRPTR) c->c_Name, (CONST_STRPTR) msg->Name ) ) {
				debug("found '%s'\n", c->c_Name );
				return( (IPTR) c );
			}
		} else { /* no channel specified, to we search for a server channel */
			if( c->c_Flags & CHANNELF_SERVER ) {
				return( (IPTR) c );
			}
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelAlloc()
**
*/

/*************************************************************************/

static ULONG MM_ChannelAlloc( struct IClass *cl, Object *obj, struct MP_NETWORK_CHANNELALLOC *msg )
{
struct Server   *s = msg->Server;
struct Channel      *c = NULL;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( !( msg->Name ) || !(msg->Name[0]) || !( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELFIND, s, msg->Name ) ) ) {
		if( ( msg->Name[0] ) ) {
			if( ( c = AllocVec( sizeof( struct Channel ), MEMF_ANY|MEMF_CLEAR ) ) ) {
				NEWLIST( &c->c_ChatLogList );
				NEWLIST( &c->c_ChatNickList );
				AddTail( &s->s_ChannelList, (struct Node *) c );
				if( msg->Name ) {
					strcpy( c->c_Name, msg->Name );
				}
			}
		}
	}
	return( (IPTR) c );
}
/* \\\ */
/* /// MM_ChannelFree()
**
*/

/*************************************************************************/

static ULONG MM_ChannelFree( struct IClass *cl, Object *obj, struct MP_NETWORK_CHANNELFREE *msg )
{
struct Channel      *c  = msg->Channel;
struct Node *node;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( c ) {
		DoMethod( _app(obj), MM_APPLICATION_CHANNELREMOVE, c );
		/* remove all chat log entries */
		while( ( node = (APTR) c->c_ChatLogList.lh_Head )->ln_Succ ) {
			DoMethod( obj, MM_NETWORK_CHATLOGENTRYFREE, node );
		}
		while( ( node = (APTR) c->c_ChatNickList.lh_Head )->ln_Succ ) {
			DoMethod( obj, MM_NETWORK_CHATNICKENTRYFREE, c, node );
		}

		if( c->c_Topic ) {
			FreeVec( c->c_Topic );
		}
		Remove( (struct Node *) c );
		FreeVec( c );
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ChatLogEntryFree()
**
*/

/*************************************************************************/

static ULONG MM_ChatLogEntryFree( struct IClass *cl, Object *obj, struct MP_NETWORK_CHATLOGENTRYFREE *msg )
{
struct ChatLogEntry *cle;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cle = msg->ChatLogEntry ) ) {
		if( cle->cle_Succ && cle->cle_Pred ) {
			Remove( ( struct Node *) cle );
		}
		FreeVec( cle );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChatLogEntryAdd()
**
** Depending on user settings and the given chat log entry, this function
** finds the desired channel and links the log entry to it.
*/

static ULONG MM_ChatLogEntryAdd( struct IClass *cl, Object *obj, struct MP_NETWORK_CHATLOGENTRYADD *msg )
{
struct Server  *s;
struct Channel *c;
struct ChatLogEntry *cle = NULL;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( s = msg->Server ) ) {
/*
** find output channel
*/
		c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELFIND, s, msg->Channel );
		if( !c->c_Succ ) {
			/* channel not found, so use server tab */
			for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; c = c->c_Succ ) {
				if( c->c_Flags & CHANNELF_SERVER ) {
					break;
				}
			}
		}
		if( !c->c_Succ ) { /* still no channel, then use the first we find */
			for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; ) {
				break;
			}
		}
/*
** add to channel
*/
		if( c->c_Succ ) { /* no channel no output */
			if( ( cle = AllocVec( sizeof( struct ChatLogEntry ) + strlen( msg->Message ) + 1, MEMF_ANY ) ) ) {
				cle->cle_Pen = msg->Pen;
				strcpy( cle->cle_Message, msg->Message );
				AddTail( &c->c_ChatLogList, (struct Node *) cle );
			}
		}
	}
	return( (IPTR) cle );
}
/* \\\ */

/* /// MM_ChatNickEntryAlloc()
**
*/

/*************************************************************************/

static ULONG MM_ChatNickEntryAlloc( struct IClass *cl, Object *obj, struct MP_NETWORK_CHATNICKENTRYALLOC *msg )
{
struct ChatNickEntry *cne;
struct Channel *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( c = msg->Channel ) ) {
		for( cne = (APTR) c->c_ChatNickList.lh_Head ; cne->cne_Succ ; cne = cne->cne_Succ ) {
			if( !Stricmp( (CONST_STRPTR) cne->cne_Nick, (CONST_STRPTR) msg->NickName ) ) {
				return( (IPTR) cne ); /* no dupes */
			}
		}
	}
	if( ( cne = AllocVec( sizeof( struct ChatNickEntry ) + strlen( msg->NickName ) + 1, MEMF_ANY ) ) ) {
		if( c ) {
			AddTail( &c->c_ChatNickList, (struct Node *) cne );
		}
		strcpy( cne->cne_Nick, msg->NickName );
	}
	return( (IPTR) cne );
}
/* \\\ */
/* /// MM_ChatNickEntryFree()
**
*/

/*************************************************************************/

static ULONG MM_ChatNickEntryFree( struct IClass *cl, Object *obj, struct MP_NETWORK_CHATNICKENTRYFREE *msg )
{
struct ChatNickEntry *cne;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cne = msg->ChatNickEntry ) ) {
		if( cne->cne_Succ && cne->cne_Pred ) {
			Remove( ( struct Node *) cne );
		}
		DoMethod( _app(obj), MM_APPLICATION_CHANNELNICKREMOVE, msg->Channel, cne );
		FreeVec( cne );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_Network_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_Network_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                       ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                         : return( OM_Dispose                   ( cl, obj, (APTR) msg ) );

		case OM_SET                             : return( OM_Set                       ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_WAITSELECT              : return( MM_WaitSelect                ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERMESSAGERECEIVED   : return( MM_ServerMessageReceived     ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERMESSAGESENDMSG    : return( MM_ServerMessageSendMsg      ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERMESSAGESENDPROC   : return( MM_ServerMessageSendProc     ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERMESSAGEPARSEBEGIN : return( MM_ServerMessageParseBegin   ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERMESSAGEPARSEEND   : return( MM_ServerMessageParseEnd     ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERMESSAGEPROCESS    : return( MM_ServerMessageProcess      ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_SERVERSOCKETINIT        : return( MM_ServerSocketInit          ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERSOCKETCLOSE       : return( MM_ServerSocketClose         ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_SERVERFIND              : return( MM_ServerFind                ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERALLOC             : return( MM_ServerAlloc               ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERFREE              : return( MM_ServerFree                ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERCONNECT           : return( MM_ServerConnect             ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERDISCONNECT        : return( MM_ServerDisconnect          ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERCONNECTAUTO       : return( MM_ServerConnectAuto         ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERLOGIN             : return( MM_ServerLogin               ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERAUTOJOIN          : return( MM_ServerAutoJoin            ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_SERVERRECEIVEDATA       : return( MM_ServerReceiveData         ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERSENDDATA          : return( MM_ServerSendData            ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_CHANNELFIND             : return( MM_ChannelFind               ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHANNELALLOC            : return( MM_ChannelAlloc                 ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHANNELFREE             : return( MM_ChannelFree               ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_CHATLOGENTRYFREE        : return( MM_ChatLogEntryFree          ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHATLOGENTRYADD         : return( MM_ChatLogEntryAdd           ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_CHATNICKENTRYALLOC      : return( MM_ChatNickEntryAlloc        ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHATNICKENTRYFREE       : return( MM_ChatNickEntryFree         ( cl, obj, (APTR) msg ) );

    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_Network_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_Network_InitClass( void )
{
	appclasses[ CLASSID_NETWORK ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_Area, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_Network_Dispatcher) );
	return( appclasses[ CLASSID_NETWORK ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_Network_DisposeClass()
**
*/

/*************************************************************************/

void MCC_Network_DisposeClass( void )
{
	if( appclasses[ CLASSID_NETWORK ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_NETWORK ] );
		appclasses[ CLASSID_NETWORK ] = NULL;
    }
}
/* \\\ */



