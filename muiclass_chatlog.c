/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_chatlog.c
*/

//#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "locale.h"
#include "functions.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingscolor.h"
#include "muiclass_chatlog.h"
#include "muiclass_network.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
WID_SETTINGS = 0,
GID_LAST
};

/*
** data used by this class
*/

#define DISPLAYBUFFER_SIZEOF 0x400

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	LONG                   mcc_MUIPen[ PEN_NUMBEROF ];
	LONG                   mcc_Pen[ PEN_NUMBEROF ];
	ULONG                  mcc_PenRGB[ PEN_NUMBEROF ];
	char                   mcc_DisplayBuffer[ DISPLAYBUFFER_SIZEOF ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
//Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *)DoSuperNew( cl, obj, TAG_DONE ) ) ) {

//		  struct mccdata *mccdata = INST_DATA( cl, obj );

//		  CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object*) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );

	DoMethod( obj, MM_CHATLOG_PENSOBTAIN  );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */
/* /// OM_Cleanup()
**
*/

/*************************************************************************/

static ULONG OM_Cleanup( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATLOG_PENSRELEASE );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatLogEntry *cle;

	if( ( cle = msg->entry ) ) {
		sprintf( mccdata->mcc_DisplayBuffer, "\033P[%ld]%s", mccdata->mcc_Pen[ cle->cle_Pen ], cle->cle_Message );
		*msg->strings = (STRPTR) mccdata->mcc_DisplayBuffer;
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ShowLastLine()
**
*/

/*************************************************************************/

static ULONG MM_ShowLastLine( struct IClass *cl, Object *obj, struct MP_CHATLOG_SHOWLASTLINE *msg )
{
IPTR visible, top, total;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	/* this magic code checks if the last line was shown before adding our entry, if
	** yes we move it down to make the new line visible, if not, we leave the list
	** position as the user may wants to read something up there
	**
	** The second to last line cannot be fixuated, but that shouldn t be a problem
	*/

	visible = MUIGetVar( obj, MUIA_NList_Visible );
	top     = MUIGetVar( obj, MUIA_NList_First   );
	total   = MUIGetVar( obj, MUIA_NList_Entries );

	if( msg->Force || ( ( top + visible ) >= total - 1 ) ) {
		SetAttrs( obj, MUIA_NList_First, total - visible + 1, TAG_DONE );
	}
	return( 0 );
}
/* \\\ */

/* /// MM_PensObtain()
**
*/

/*************************************************************************/

static ULONG MM_PensObtain( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct MUI_PenSpec *penspec;
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; i < PEN_NUMBEROF ; i++ ) {
		if( ( penspec = (APTR) LRC( OID_SETTINGSCOLOR + i ) ) ) {
			mccdata->mcc_MUIPen[ i ] = MUI_ObtainPen( muiRenderInfo( obj ), penspec, 0 );
			mccdata->mcc_Pen[ i ]    = MUIPEN( mccdata->mcc_MUIPen[ i ] );
			mccdata->mcc_PenRGB[ i ] = MUIPenSpecToRGB( obj, penspec );
		}
	}
	#if ENABLE_NLIST /* NList is not using the standard background tag */
		SetAttrs( obj, MUIA_NList_ListBackground, LRC( OID_SETTINGSCOLOR + PEN_LOGLISTBACKGROUND ), TAG_DONE );
	#else
		SetAttrs( obj, MUIA_Background, LRC( OID_SETTINGSCOLOR + PEN_LOGLISTBACKGROUND ), TAG_DONE );
	#endif

	return( 0 );
}
/* \\\ */
/* /// MM_PensRelease()
**
*/

/*************************************************************************/

static ULONG MM_PensRelease( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; i < PEN_NUMBEROF ; i++ ) {
		MUI_ReleasePen( muiRenderInfo( obj ), mccdata->mcc_MUIPen[ i ] );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_PensUpdate()
**
*/

/*************************************************************************/

static ULONG MM_PensUpdate( struct IClass *cl, Object *obj, Msg *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MM_CHATLOG_PENSOBTAIN  );
	DoMethod( obj, MM_CHATLOG_PENSRELEASE );

	return( 0 );
}
/* \\\ */

/* /// MM_Open()
**
*/

/*************************************************************************/

static ULONG MM_Open( struct IClass *cl, Object *obj, struct MP_CHATLOG_OPEN *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Channel *c;
struct Server  *s;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( !mccdata->mcc_ClassObjects[ WID_SETTINGS ] ) {
		mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object*) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );
	}


	if( ( c = msg->Channel ) && ( s = msg->Server ) ) {
		if( ( LRC( OID_LOG_ACTIVEPUBLIC  ) &&  ( c->c_Flags & CHANNELF_PUBLIC ) ) ||
			( LRC( OID_LOG_ACTIVEPRIVATE ) && !( c->c_Flags & CHANNELF_PUBLIC ) ) ){

			struct ChatLogEntry *cle;
			#define FILENAME_SIZEOF 0x200
			char filename[ FILENAME_SIZEOF ];
			#define BUFFER_SIZEOF 0x1000
			char *buffer;
			long len, line, maxline;

			sprintf( filename, "ram:%s_%s_cur", s->s_Name, c->c_Name );

			DoMethod( obj, MM_CHATLOG_CLOSE, c );

			line = 0;
			maxline = ( c->c_Flags & CHANNELF_PUBLIC ) ? LRC( OID_LOG_RECOVERPUBLIC ) : LRC( OID_LOG_RECOVERPRIVATE );

			if( ( c->c_ChatLogFile = Open( (_ub_cs) filename, MODE_READWRITE ) ) ) {
				if( ( buffer = AllocVec( BUFFER_SIZEOF, MEMF_ANY|MEMF_CLEAR ) ) ) {
					while( FGets( c->c_ChatLogFile, (STRPTR) buffer, BUFFER_SIZEOF - 1 ) ) {
						VPrintf("line is '%s'\n", &buffer );
						while( ( len = strlen( buffer ) ) > 0 ) {
							len--;
							if( ( buffer[ len ] == '\n' ) || ( buffer[ len ] == '\r' ) ) {
								buffer[ len ] = 0x00;
							} else {
								break;
							}
							DoMethod( obj, MM_CHATLOG_ENTRYALLOC, c, PEN_LOGINFO, buffer );
							line++; /* we load all lines */
							if( line > maxline ) {
								if( ( cle = (APTR) c->c_ChatLogList.lh_Head )->cle_Succ ) {
									/* but keep only a limited number */
									DoMethod( obj, MM_CHATLOG_ENTRYFREE, cle );
								}
							}
						}
					}
					FreeVec( buffer );
				}
				/* no close here, as we continue logging */
			}
		} else {
			DoMethod( obj, MM_CHATLOG_CLOSE, c );
		}
	}
	debug( "%s (%ld) %s() - Done\n", __FILE__, __LINE__, __func__ );
	return( 0 );
}
/* \\\ */
/* /// MM_Close()
**
*/

/*************************************************************************/

static ULONG MM_Close( struct IClass *cl, Object *obj, struct MP_CHATLOG_CLOSE *msg )
{
struct Channel *c;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( c = msg->Channel ) ) {
		if( c->c_ChatLogFile ) {
			Close( c->c_ChatLogFile );
			c->c_ChatLogFile = NULL;
		}
	}

	return( 0 );
}
/* \\\ */
/* /// MM_Write()
**
*/

/*************************************************************************/

static ULONG MM_Write( struct IClass *cl, Object *obj, struct MP_CHATLOG_WRITE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Channel *c;
struct ChatLogEntry *cle;

//	  debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( c = msg->Channel ) && ( cle = msg->ChatLogEntry ) ) {
		if( c->c_ChatLogFile ) {
			ULONG length;

			FPuts( c->c_ChatLogFile, (CONST_STRPTR) cle->cle_Message );
			FPuts( c->c_ChatLogFile, (CONST_STRPTR) "\n" );

			/* check size limit */
			if( LRC( OID_LOG_SPLIT ) ) { /* splitting enabled ? */
				if( ( length = Dos_GetSizeHandle( c->c_ChatLogFile ) ) ) {
					length = length	/ ( 1024 * 1024 );
					if( length > LRC( OID_LOG_SPLITLIMIT ) ) { /* limit reached? */
						debug("%s (%ld) %s() - Splitting log\n", __FILE__, __LINE__, __func__ );
					}
				}
			}
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_EntryAlloc()
**
*/

/*************************************************************************/

static ULONG MM_EntryAlloc( struct IClass *cl, Object *obj, struct MP_CHATLOG_ENTRYALLOC *msg )
{
struct Channel *c;
struct ChatLogEntry *cle = NULL;

//	  debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( c = msg->Channel ) && c->c_Succ ) {
		if( ( cle = AllocVec( sizeof( struct ChatLogEntry ) + strlen( msg->Message ) + 1, MEMF_ANY ) ) ) {
			cle->cle_Pen = msg->Pen;
			strcpy( cle->cle_Message, msg->Message );
			AddTail( &c->c_ChatLogList, (struct Node *) cle );
		}
	}
	return( (IPTR) cle );
}
/* \\\ */
/* /// MM_EntryFree()
**
*/

/*************************************************************************/

static ULONG MM_EntryFree( struct IClass *cl, Object *obj, struct MP_CHATLOG_ENTRYFREE *msg )
{
struct ChatLogEntry *cle;

//	  debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( cle = msg->ChatLogEntry ) ) {
		if( cle->cle_Succ && cle->cle_Pred ) {
			Remove( ( struct Node *) cle );
		}
		FreeVec( cle );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatLog_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatLog_Dispatcher)
{
	switch ( msg->MethodID ) {
		case OM_NEW                          : return( OM_New          ( cl, obj, (APTR) msg ) );
		case MUIM_Setup                      : return( OM_Setup        ( cl, obj, (APTR) msg ) );
		case MUIM_Cleanup                    : return( OM_Cleanup      ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display      ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_SHOWLASTLINE         : return( MM_ShowLastLine ( cl, obj, (APTR) msg ) );

		case MM_CHATLOG_PENSOBTAIN           : return( MM_PensObtain   ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_PENSRELEASE          : return( MM_PensRelease  ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_PENSUPDATE           : return( MM_PensUpdate   ( cl, obj, (APTR) msg ) );

		case MM_CHATLOG_OPEN                 : return( MM_Open         ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_CLOSE                : return( MM_Close        ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_WRITE                : return( MM_Write        ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_ENTRYALLOC           : return( MM_EntryAlloc   ( cl, obj, (APTR) msg ) );
		case MM_CHATLOG_ENTRYFREE            : return( MM_EntryFree    ( cl, obj, (APTR) msg ) );

    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChatLog_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChatLog_InitClass( void )
{
	appclasses[ CLASSID_CHATLOG ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_ChatLog_Dispatcher) );
	return( appclasses[ CLASSID_CHATLOG ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChatLog_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChatLog_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHATLOG ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHATLOG ] );
		appclasses[ CLASSID_CHATLOG ] = NULL;
    }
}
/* \\\ */

