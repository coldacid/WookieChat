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

#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "includes.h"
#include "intern.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowchat.h"
#include "muiclass_network.h"

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
	struct List            mcc_ConnectedList;
	fd_set                 mcc_ReadMaster;       // master file descriptor list
	fd_set                 mcc_WriteMaster;      // master file descriptor list
	int                    mcc_FDMax;            // maximum file descriptor number

};

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg )
{
	if( ( obj = (Object*) DoSuperNew( cl, obj, TAG_DONE ) ) ) {
		struct mccdata *mccdata = INST_DATA( cl, obj );
		
		SetAttrs( obj, TAG_MORE, msg->ops_AttrList );

		NEWLIST( &mccdata->mcc_ConnectedList ); /* init list */

		/* init defaults */
		mccdata->mcc_FDMax = -1;

	}
	return( (ULONG) obj );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s;

	while( (s = (APTR) mccdata->mcc_ConnectedList.lh_Head)->s_Succ ) {
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
/* /// MM_ServerAlloc()
**
*/

/*************************************************************************/

static ULONG MM_ServerAlloc( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERALLOC *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry  *se;
struct NickEntry    *ne;
struct ChannelEntry *ce;
struct Server   *s = NULL;
struct Nick         *n;
struct Channel      *c;

	if( ( se = msg->ServerEntry ) ) {
		if( ( c = AllocVec( sizeof( struct Server ), MEMF_ANY|MEMF_CLEAR ) ) ) {
			NEWLIST( &s->s_ChannelList );
			NEWLIST( &s->s_NickList );
			AddTail( &mccdata->mcc_ConnectedList, (struct Node *) c );
			/* we cannot link to server entry as it may be removed by user during
			** runtime, so we spy important data */
			strcpy( s->s_Name    , se->se_Name );
			strcpy( s->s_Address , se->se_Address );
			strcpy( s->s_Charset , se->se_Charset );
			strcpy( s->s_Password, se->se_Password );
			if( !( s->s_Port = se->se_Port ) ) {
				s->s_Port = 6667;
			}
			s->s_State = SVRSTATE_NOTCONNECTED;
			for( ce = (APTR) se->se_ChannelList.lh_Head ; ce->ce_Succ ; ce = ce->ce_Succ ) {
				if( ( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELALLOC, s, ce->ce_Name ) ) ) {
					strcpy( c->c_Password, ce->ce_Password );
				}
			}
			for( ne = (APTR) se->se_ChannelList.lh_Head ; ne->ne_Succ ; ne = ne->ne_Succ ) {
				if( ( n = AllocVec( sizeof( struct Channel ), MEMF_ANY ) ) ) {
					AddTail( &s->s_NickList, (struct Node *) n );
					strcpy( n->n_Name    , ne->ne_Name     );
					strcpy( n->n_Password, ne->ne_Password );
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

	/* dissnnect if needed */

	DoMethod( obj, MM_NETWORK_SERVERDISCONNECT, s );

	/* remove all channels */
	while( ( c = (APTR) s->s_ChannelList.lh_Head )->c_Succ ) {
		DoMethod( obj, MM_NETWORK_CHANNELFREE, c );
	}

	/* remove all nicks */
	while( ( n = (APTR) s->s_NickList.lh_Head )->n_Succ ) {
		Remove( (struct Node *) n );
		FreeVec( n );
	}
	FreeVec( s );

	return( 0 );
}
/* \\\ */
/* /// MM_ServerConnect()
**
*/

/*************************************************************************/

static ULONG MM_ServerConnect( struct IClass *cl, Object *obj, struct MP_NETWORK_SERVERCONNECT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server *s = NULL;
char *result;

	if( !SocketBase ) {
		SocketBase = OpenLibrary( (_ub_cs) "bsdsocket.library", 0L );
#ifdef __amigaos4__
		ISocket = (struct SocketIFace*) GetInterface( (struct Library *) SocketBase, "main", 1, NULL );
#endif
    }
	if( SocketBase ) {
		if( ( s = msg->Server ) ) {
			struct hostent *host;
			result = (char *) DoMethod( obj, MM_NETWORK_LOGNOTECREATE, LOGTYPE_ERROR, "%s: gethostbyname() %s.", s->s_Address, LGS( MSG_DCC_FAILED ) );

			if( ( host = gethostbyname ( (APTR) s->s_Address ) ) ) { // get the host info

				memset( &( s->s_ServerSocket ), 0, sizeof( s->s_ServerSocket ) );
				s->s_ServerSocket.sin_family = AF_INET;
				s->s_ServerSocket.sin_port   = htons( s->s_Port );
				s->s_ServerSocket.sin_addr   = *( (struct in_addr *) host->h_addr );

				result = (char *) DoMethod( obj, MM_NETWORK_LOGNOTECREATE, LOGTYPE_ERROR, "%s: gethostbyname() %s.", s->s_Address, LGS( MSG_DCC_FAILED ) );

				s->s_a_socket = socket( AF_INET, SOCK_STREAM, 0 );
				if( mccdata->mcc_FDMax < s->s_a_socket ) {
					mccdata->mcc_FDMax = s->s_a_socket;
				}

				FD_CLR( s->s_a_socket, &mccdata->mcc_ReadMaster  );
				FD_CLR( s->s_a_socket, &mccdata->mcc_WriteMaster );

				//DoMethod( obj, MM_NETWORK_ADDLOG, LOGTYPE_ACTIVITY, LGS( MSG_CONNECT_TO_A_NEW_SERVER ) );

				if( s->s_a_socket != -1 ) { /* still open? */
					CloseSocket( s->s_a_socket );
				}

				if( s->s_a_socket != -1 ) {
					FD_SET( s->s_a_socket, &mccdata->mcc_ReadMaster  );
					FD_SET( s->s_a_socket, &mccdata->mcc_WriteMaster );
				}

				if( 1 /* ident */ ) {
					int yes = 1;

					if( s->ident_listen_socket != -1 ) { /* still open? */
						CloseSocket( s->ident_listen_socket );
					}
					s->ident_listen_socket = socket( AF_INET, SOCK_STREAM, 0 );
					s->ident_a_socket = -1;
	
					if( s->ident_listen_socket == -1 )
						debug("unable to create listening socket\nerror number:%li\n", Errno());
					else
						debug("dcc listen socket created, number: %li\n", s->ident_listen_socket );

					if( mccdata->mcc_FDMax < s->ident_listen_socket ) {
						mccdata->mcc_FDMax = s->ident_listen_socket;
					}

					memset( &s->ident_test, 0, sizeof( s->ident_test));
					s->ident_test.sin_family      = AF_INET; // host byte order
					s->ident_test.sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
					s->ident_test.sin_port        = htons(113); // short, network byte order

					if( setsockopt( (int) s->ident_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEADDR, &yes, (int) sizeof( yes ) ) == -1 )
						debug( "unable to reuse address\n");

					if( setsockopt( (int) s->ident_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEPORT, &yes, (int) sizeof( yes ) ) == -1 )
						debug("unable to reuse port\n");

					if( bind( s->ident_listen_socket, (struct sockaddr *) &s->ident_test, sizeof( s->ident_test ) ) == -1 )
						debug("unable to bind address to socket, error number:%li\n", Errno() );

					if( IoctlSocket( s->ident_listen_socket, FIONBIO, (char*) &yes ) < 0 )
						debug("unable to change non-blocking I/O option for socket, error number:%li\n", Errno() );

					FD_SET( s->ident_listen_socket, &mccdata->mcc_ReadMaster );

					listen( (int) s->ident_listen_socket, (int) 5 );
				}   // END of IDENTD setup

				long i = 1;

				IoctlSocket( s->s_a_socket, FIONBIO, (char*) &i );



				connect( s->s_a_socket, (struct sockaddr*) &s->s_ServerSocket, sizeof( struct sockaddr ) );

			}
			if( result ) {
				DoMethod( obj, MM_NETWORK_LOGNOTEADD, s, result );
			}
		}
	}
	return( 0 );
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
fd_set read_fds  = mccdata->mcc_ReadMaster;
fd_set write_fds = mccdata->mcc_WriteMaster;

	selectresult  = WaitSelect( mccdata->mcc_FDMax + 1, &read_fds, &write_fds, NULL, NULL, msg->SignalMask );

	if( selectresult > 0 ) {
		DoMethod( obj, MM_NETWORK_HANDLESOCKETS );
		debug("handle sockets\n");
	}
	return( *((ULONG*)msg->SignalMask ) );
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

	for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; c = c->c_Succ ) {
		if( !Stricmp( (CONST_STRPTR) c->c_Name, (CONST_STRPTR) msg->Name ) ) {
			return( (IPTR) c );
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
struct Channel      *c;

	if( !( msg->Name ) || !(msg->Name[0]) || !( c = (APTR) DoMethod( obj, MM_NETWORK_CHANNELFIND, s, msg->Name ) ) ) {
		if( (msg->Name[0] ) ) {
			if( ( c = AllocVec( sizeof( struct Channel ), MEMF_ANY|MEMF_CLEAR ) ) ) {
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

	if( c ) {
		if( c->c_ChatWindow ) {
			DoMethod( c->c_ChatWindow, MM_WINDOWCHAT_CHANNELREMOVE, c );
		}
		Remove( (struct Node *) c );
		FreeVec( c );
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
		case OM_NEW                        : return( OM_New                      ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                    : return( OM_Dispose                  ( cl, obj, (APTR) msg ) );
		case OM_SET                        : return( OM_Set                      ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_WAITSELECT         : return( MM_WaitSelect               ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERFIND     : return( MM_ServerAlloc          ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERALLOC    : return( MM_ServerAlloc          ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERFREE     : return( MM_ServerFree           ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_SERVERCONNECT  : return( MM_ServerConnect        ( cl, obj, (APTR) msg ) );

		case MM_NETWORK_CHANNELFIND        : return( MM_ChannelFind              ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHANNELALLOC       : return( MM_ChannelAlloc             ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CHANNELFREE        : return( MM_ChannelFree              ( cl, obj, (APTR) msg ) );
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




