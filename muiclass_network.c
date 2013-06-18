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
	int                    mcc_FDMax;        // maximum file descriptor number

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
//struct mccdata *mccdata = INST_DATA( cl, obj );

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
/* /// MM_ConnectAlloc()
**
*/

/*************************************************************************/

static ULONG MM_ConnectAlloc( struct IClass *cl, Object *obj, struct MP_NETWORK_CONNECTALLOC *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se;
struct Connection *c = NULL;

	if( ( se = msg->ServerEntry ) ) {
		if( ( c = AllocVec( sizeof( struct Connection ), MEMF_ANY|MEMF_CLEAR ) ) ) {
			NEWLIST( &c->c_Channels );
			AddTail( &mccdata->mcc_ConnectedList, (struct Node *) c );
			/* we cannot link to server entry as it may be removed by user during
			** runtime, so we copy important data */
			strcpy( c->c_Name    , se->se_Name );
			strcpy( c->c_Address , se->se_Address );
			strcpy( c->c_Charset , se->se_Charset );
			strcpy( c->c_Password, se->se_Password );
			if( !( c->c_Port = se->se_Port ) ) {
				c->c_Port = 6667;
			}
			c->c_State = SVRSTATE_NOTCONNECTED;
		}
	}
	return( (IPTR) c );
}
/* \\\ */
/* /// MM_Connect()
**
*/

/*************************************************************************/

static ULONG MM_Connect( struct IClass *cl, Object *obj, struct MP_NETWORK_CONNECT *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se;
struct Connection *c = NULL;
char *result;

	if( !SocketBase ) {
		SocketBase = OpenLibrary( (_ub_cs) "bsdsocket.library", 0L );
#ifdef __amigaos4__
		ISocket = (struct SocketIFace*) GetInterface( (struct Library *) SocketBase, "main", 1, NULL );
#endif
    }
	if( SocketBase ) {
		if( ( se = msg->ServerEntry ) ) {
			if( ( c = (APTR) DoMethod( obj, MM_NETWORK_CONNECTALLOC, se ) ) ) {
				struct hostent *host;
				result = (char *) DoMethod( obj, MM_NETWORK_LOGNOTECREATE, LOGTYPE_ERROR, "%s: gethostbyname() %s.", c->c_Address, LGS( MSG_DCC_FAILED ) );

				if( ( host = gethostbyname ( (APTR) c->c_Address ) ) ) { // get the host info

					memset( &( c->c_ServerSocket ), 0, sizeof( c->c_ServerSocket ) );
					c->c_ServerSocket.sin_family = AF_INET;
					c->c_ServerSocket.sin_port   = htons( c->c_Port );
					c->c_ServerSocket.sin_addr   = *( (struct in_addr *) host->h_addr );

					result = (char *) DoMethod( obj, MM_NETWORK_LOGNOTECREATE, LOGTYPE_ERROR, "%s: gethostbyname() %s.", c->c_Address, LGS( MSG_DCC_FAILED ) );

					c->c_a_socket = socket( AF_INET, SOCK_STREAM, 0 );
					if( mccdata->mcc_FDMax < c->c_a_socket ) {
						mccdata->mcc_FDMax = c->c_a_socket;
					}

					FD_CLR( c->c_a_socket, &mccdata->mcc_ReadMaster  );
					FD_CLR( c->c_a_socket, &mccdata->mcc_WriteMaster );

					//DoMethod( obj, MM_NETWORK_ADDLOG, LOGTYPE_ACTIVITY, LGS( MSG_CONNECT_TO_A_NEW_SERVER ) );

					if( c->c_a_socket != -1 ) { /* still open? */
						CloseSocket( c->c_a_socket );
					}

					if( c->c_a_socket != -1 ) {
						FD_SET( c->c_a_socket, &mccdata->mcc_ReadMaster  );
						FD_SET( c->c_a_socket, &mccdata->mcc_WriteMaster );
					}

					if( 1 /* ident */ ) {
						int yes = 1;

						if( c->ident_listen_socket != -1 ) { /* still open? */
							CloseSocket( c->ident_listen_socket );
						}
						c->ident_listen_socket = socket( AF_INET, SOCK_STREAM, 0 );
						c->ident_a_socket = -1;
	
						if( c->ident_listen_socket == -1 )
							debug("unable to create listening socket\nerror number:%li\n", Errno());
						else
							debug("dcc listen socket created, number: %li\n", c->ident_listen_socket );

						if( mccdata->mcc_FDMax < c->ident_listen_socket ) {
							mccdata->mcc_FDMax = c->ident_listen_socket;
						}

						memset(& c->ident_test, 0, sizeof( c->ident_test));
						c->ident_test.sin_family      = AF_INET; // host byte order
						c->ident_test.sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
						c->ident_test.sin_port        = htons(113); // short, network byte order

						if( setsockopt( (int) c->ident_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEADDR, &yes, (int) sizeof( yes ) ) == -1 )
							debug( "unable to reuse address\n");

						if( setsockopt( (int) c->ident_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEPORT, &yes, (int) sizeof( yes ) ) == -1 )
							debug("unable to reuse port\n");

						if( bind( c->ident_listen_socket, (struct sockaddr *) &c->ident_test, sizeof( c->ident_test ) ) == -1 )
							debug("unable to bind address to socket, error number:%li\n", Errno() );

						if( IoctlSocket( c->ident_listen_socket, FIONBIO, (char*) &yes ) < 0 )
							debug("unable to change non-blocking I/O option for socket, error number:%li\n", Errno() );

						FD_SET( c->ident_listen_socket, &mccdata->mcc_ReadMaster );

						listen( (int) c->ident_listen_socket, (int) 5 );
					}   // END of IDENTD setup

					long i = 1;

					IoctlSocket( c->c_a_socket, FIONBIO, (char*) &i );



					connect( c->c_a_socket, (struct sockaddr*) &c->c_ServerSocket, sizeof( struct sockaddr ) );

				}
				if( result ) {
					DoMethod( obj, MM_NETWORK_LOGNOTEADD, c, result );
				}
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
		case MM_NETWORK_CONNECTALLOC       : return( MM_ConnectAlloc             ( cl, obj, (APTR) msg ) );
		case MM_NETWORK_CONNECT            : return( MM_Connect                  ( cl, obj, (APTR) msg ) );
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




