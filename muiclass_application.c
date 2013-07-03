/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_application.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <SDI_hook.h>
#include <string.h>
#include <stdio.h>

#include "system.h"
#include "locale.h"
#include "functions.h"
#include "muiclass.h"
#include "muiclass_audio.h"
#include "muiclass_application.h"
#include "muiclass_network.h"
#include "muiclass_chatlog.h"
#include "muiclass_windowchat.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowquicksetup.h"
#include "muiclass_windowabout.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowignorelist.h"
#include "muiclass_windowurlgrabber.h"
#include "version.h"

/*************************************************************************/

#define READARGS_TEMPLATE "ICONIFY/S"

enum{
READARGS_ICONIFY = 0,
READARGS_LAST
};

/*
** objects used by this class
*/

enum
{
WID_QUIT = 0,
WID_ABOUT,
WID_SETTINGS,
WID_IGNORELIST,
WID_URLGRABBER,
WID_QUICKSETUP,
GID_AUDIO,
GID_NETWORK,
GID_CHATLOG,
GID_LAST
};

/*
** data used by this class
*/
#define LASTACTIVE_MAX 100 /* we keep track of the last n windows */
struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	struct DiskObject     *mcc_DiskObject;
	IPTR                   mcc_ReadargsArray[ READARGS_LAST ];
	Object                *mcc_LastActiveChat[ LASTACTIVE_MAX ];
	ULONG                  mcc_InstanceCount; /* used to give each chat window a unique ObjectID */
};


/*************************************************************************/

/* /// OM_New()
**
*/


static STRPTR classlist[] = {
#if ENABLE_NLIST
		(STRPTR)"NList.mcc",
		(STRPTR)"NListview.mcc",
		(STRPTR)"NListtree.mcc",
#endif
		NULL };

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = (Object *) DoSuperNew( cl, obj,
		        MUIA_Application_UsedClasses, classlist,
				MUIA_Application_Author     , (char*) AUTHORNAME,
				MUIA_Application_Base       , (char*) APPLICATIONNAME "_NEW2",
				MUIA_Application_Title      , (char*) APPLICATIONNAME "_NEW2",
				MUIA_Application_Version    , (char*) VERSION_MUI,
				MUIA_Application_Copyright  , (char*) VERSION_COPYRIGHT,
				MUIA_Application_Description, (char*) LGS( MSG_MUICLASS_APPLICATION_DESCRIPTION ),
				MUIA_Application_SingleTask , FALSE,
				MUIA_Application_Window     , objs[ WID_QUIT          ] = WindowQuitObject      , End,
				MUIA_Application_Window     , objs[ WID_ABOUT         ] = WindowAboutObject     , End,
				MUIA_Application_Window     , objs[ WID_SETTINGS      ] = WindowSettingsObject  , End,
				MUIA_Application_Window     , objs[ WID_IGNORELIST    ] = WindowIgnoreListObject, End,
				MUIA_Application_Window     , objs[ WID_URLGRABBER    ] = WindowURLGrabberObject, End,
				MUIA_Application_Window     , objs[ WID_QUICKSETUP    ] = WindowQuickSetupObject, End,
				/* this is just a dummy to store/handle our non visible classes, without additional code */
				MUIA_Application_Window     , WindowObject, MA_APPLICATION_CLASSID, -1,
								WindowContents, VGroup,
									Child   , objs[ GID_AUDIO   ] = AudioObject , End,
									Child   , objs[ GID_NETWORK ] = NetworkObject , End,
									Child   , objs[ GID_CHATLOG ] = ChatLogObject , End,
								End,
					End,
				TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		SetAttrs( objs[ GID_NETWORK ], MA_NETWORK_OBJECTSETTINGS, objs[ WID_SETTINGS ], MA_NETWORK_OBJECTAUDIO, objs[ GID_AUDIO    ], MA_NETWORK_OBJECTCHATLOG, objs[ GID_CHATLOG ], TAG_DONE );
		SetAttrs( objs[ GID_AUDIO   ], MA_AUDIO_OBJECTSETTINGS  , objs[ WID_SETTINGS ], TAG_DONE );

		/* load settings */
		DoMethod( obj, MUIM_Application_Load, MUIV_Application_Load_ENVARC );
		DoMethod( obj, MUIM_Application_Load, MUIV_Application_Load_ENV );
		return( (IPTR) obj );
	}
	return( (IPTR) NULL );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static IPTR OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	SetAttrs( obj, MUIA_Application_DiskObject, 0, TAG_DONE );
	if( mccdata->mcc_DiskObject ) {
		FreeDiskObject( mccdata->mcc_DiskObject );
	}
	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static IPTR OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_OBJECTNETWORK          : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ GID_NETWORK       ] ; return( TRUE );
		case MA_APPLICATION_OBJECTWINDOWQUIT       : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_QUIT          ] ; return( TRUE );
		case MA_APPLICATION_OBJECTWINDOWABOUT      : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_ABOUT         ] ; return( TRUE );
		case MA_APPLICATION_OBJECTWINDOWSETTINGS   : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_SETTINGS      ] ; return( TRUE );
		case MA_APPLICATION_OBJECTWINDOWIGNORELIST : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_IGNORELIST    ] ; return( TRUE );
		case MA_APPLICATION_OBJECTWINDOWURLGRABBER : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_URLGRABBER    ] ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */

/* /// MM_Startup()
**
*/

/*************************************************************************/

static IPTR MM_Startup( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	mccdata->mcc_DiskObject	= Icon_GetPutDiskObject( NULL );
	if( mccdata->mcc_DiskObject ) {
		SetAttrs( obj, MUIA_Application_DiskObject, mccdata->mcc_DiskObject, TAG_DONE );
	}

	if( !wbmessage ) {
		struct RDArgs *readargs;
		if( !( readargs = ReadArgs( (_ub_cs) READARGS_TEMPLATE, (LONG*) mccdata->mcc_ReadargsArray, NULL ) ) ) {
			Dos_ShowFailure();
		} else {
			/* insert arg handling */
			if( readargs ) {
				FreeArgs( readargs );
			}
		}
	}

	/* auto connect or open wizard */
	if( !( DoMethod( mccdata->mcc_ClassObjects[ WID_QUICKSETUP ], MM_WINDOWQUICKSETUP_CHECKSETTINGS ) ) ) {
		DoMethod( obj, MM_APPLICATION_WINDOWCHATNEW );
		DoMethod( mccdata->mcc_ClassObjects[ GID_NETWORK ], MM_NETWORK_SERVERCONNECTAUTO );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_Application_Load()
**
*/

/*************************************************************************/

static IPTR MM_Application_Load( struct IClass *cl, Object *obj, Msg msg )
{
ULONG rc = DoSuperMethodA( cl, obj, (Msg) msg );
//struct mccdata *mccdata = INST_DATA( cl, obj );

//	  DoMethod( mccdata->mcc_ClassObjects[ WID_CHAT ], MM_WINDOWCHAT_COLORCHANGE );

	return( rc );
}
/* \\\ */
/* /// MM_WindowFind()
**
** Check if a window object still exists. This should never fail
** and was just implemented to add some protection and find errors
** in chat handling code.
*/

/*************************************************************************/

static IPTR MM_WindowFind( struct IClass *cl UNUSED , Object *obj, struct MP_APPLICATION_WINDOWFIND *msg )
{

	FORCHILD( obj, MUIA_Application_WindowList ) {
		if( msg->Window == child ) { /* child is in window list */
			return( (IPTR) child );
		}
	} NEXTCHILD

	debug("###### something is broken, found invalid window object ###########\n");

	return( 0 );
}
/* \\\ */
/* /// MM_WindowDispose()
**
*/

/*************************************************************************/

static IPTR MM_WindowDispose( struct IClass *cl UNUSED, Object *obj, struct MP_APPLICATION_WINDOWDISPOSE *msg )
{
Object *win;

	if( ( win = (Object *) DoMethod( obj, MM_APPLICATION_WINDOWFIND, msg->Window ) ) ) {
		DoMethod( _app(obj), OM_REMMEMBER, win );
		MUI_DisposeObject( win );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_WindowChatNew()
**
*/

/*************************************************************************/

static IPTR MM_WindowChatNew( struct IClass *cl UNUSED, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
Object *win;

	if( ( win = WindowChatObject,
			MA_WINDOWCHAT_OBJECTNETWORK , mccdata->mcc_ClassObjects[ GID_NETWORK  ],
			MA_WINDOWCHAT_OBJECTSETTINGS, mccdata->mcc_ClassObjects[ WID_SETTINGS ],
			MA_WINDOWCHAT_OBJECTQUIT    , mccdata->mcc_ClassObjects[ WID_QUIT     ],
			End ) ) {
		DoMethod( obj, OM_ADDMEMBER, win );
		if( win ) {
			DoMethod( obj, MM_APPLICATION_CHATSETACTIVE, win );
			DoMethod( win, MUIM_Notify, MUIA_Window_Activate, TRUE, obj, 2, MM_APPLICATION_CHATSETACTIVE, win );
			SetAttrs( win, MUIA_Window_Open, TRUE, MUIA_ObjectID, OID_WINDOWCHAT + mccdata->mcc_InstanceCount, TAG_DONE );
			mccdata->mcc_InstanceCount++;
		}
	}
	return( (IPTR) win );
}
/* \\\ */

/* /// MM_ChatSetActive()
**
*/

/*************************************************************************/

static IPTR MM_ChatSetActive( struct IClass *cl UNUSED , Object *obj, struct MP_APPLICATION_CHATSETACTIVE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	DoMethod( obj, MM_APPLICATION_CHATREMOVE, msg->Window );

	for( i = 0 ; i < ( LASTACTIVE_MAX - 1 ) ; i++ ) {
		mccdata->mcc_LastActiveChat[ ( LASTACTIVE_MAX - 1 ) - i  ] = mccdata->mcc_LastActiveChat[ ( LASTACTIVE_MAX - 2 ) - i  ];
	}
	mccdata->mcc_LastActiveChat[ 0 ] = msg->Window;

	return( 0 );
}
/* \\\ */
/* /// MM_ChatRemove()
**
*/

/*************************************************************************/

static IPTR MM_ChatRemove( struct IClass *cl UNUSED , Object *obj, struct MP_APPLICATION_CHATREMOVE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i, o;

	/* we purge all windows that no longer exist */

	for( i = 0 ; i < LASTACTIVE_MAX ; i++ ) {
		if( mccdata->mcc_LastActiveChat[ i ] ) {
			if( !DoMethod( obj, MM_APPLICATION_WINDOWFIND, mccdata->mcc_LastActiveChat[ i ] ) ) {
				mccdata->mcc_LastActiveChat[ i ] = NULL; /* not found, so kill it */
			} else {
			/* if it is the window to kill, kill it */
				if( msg->Window == mccdata->mcc_LastActiveChat[ i ] ) {
					mccdata->mcc_LastActiveChat[ i ] = NULL;
					/* we could break here, but we loop to purge/find wrong entries */
				}
			}
		}
	}
	/* clean array */
	for( o = 0, i = 0 ; i < LASTACTIVE_MAX ; i++ ) {
		if( mccdata->mcc_LastActiveChat[ i ] ) {
			mccdata->mcc_LastActiveChat[ o++ ] = mccdata->mcc_LastActiveChat[ i ];
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChatFindActive()
**
*/

/*************************************************************************/

static IPTR MM_ChatFindActive( struct IClass *cl UNUSED , Object *obj, struct MP_APPLICATION_CHATFINDACTIVE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; i < LASTACTIVE_MAX ; i++ ) {
		if( mccdata->mcc_LastActiveChat[ i ] ) {
			/* just for savty */
			if( DoMethod( obj, MM_APPLICATION_WINDOWFIND, mccdata->mcc_LastActiveChat[ i ] ) ) {
				return( (IPTR) mccdata->mcc_LastActiveChat[ i ] );
			}
		}
	}

	return( 0 );
}
/* \\\ */

/* /// MM_MessageReceived()
**
** We distribute this method to all chat windows.
*/

/*************************************************************************/

static IPTR MM_MessageReceived( struct IClass *cl, Object *obj, struct MP_APPLICATION_MESSAGERECEIVED *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Server  *s;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( s = msg->Server ) ) {
		struct Channel *c;

/*
** find output channel
*/
		c = (APTR) DoMethod( mccdata->mcc_ClassObjects[ GID_NETWORK ], MM_NETWORK_CHANNELFIND, s, msg->ChannelName );
		if( !c->c_Succ ) {
			/* channel not found, so use server tab */
			for( c = (APTR) s->s_ChannelList.lh_Head ; c->c_Succ ; c = c->c_Succ ) {
				if( c->c_Flags & CHANNELF_SERVER ) {
					break;
				}
			}
		}
		if( c->c_Succ ) {
			struct ChatLogEntry *cle;
			cle = (APTR) DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MM_CHATLOG_ENTRYALLOC, c, msg->Pen, msg->Message );
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MM_CHATLOG_WRITE, c, cle );

			FORCHILD( obj, MUIA_Application_WindowList ) {
				if( MUIGetVar( child, MA_APPLICATION_CLASSID ) == CLASSID_WINDOWCHAT ) {
					DoMethod( child, MM_WINDOWCHAT_MESSAGERECEIVED, cle );
				}
			} NEXTCHILD
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelAdd()
**
*/

/*************************************************************************/

static IPTR MM_ChannelAdd( struct IClass *cl, Object *obj, struct MP_APPLICATION_CHANNELADD *msg )
{
Object *windowchat;

	if( !( windowchat = (Object *) DoMethod( obj, MM_APPLICATION_CHATFINDACTIVE ) ) ) {
		FORCHILD( obj, MUIA_Application_WindowList ) {
			if( MUIGetVar( child, MA_APPLICATION_CLASSID ) == CLASSID_WINDOWCHAT ) {
				windowchat = child;
				break;
			}
		} NEXTCHILD
		if( !windowchat ) {
			windowchat = (Object *) DoMethod( obj, MM_APPLICATION_WINDOWCHATNEW );
		}
	}
	if( windowchat ) {
		DoMethod( windowchat, MM_WINDOWCHAT_CHANNELADD, msg->Channel );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelRemove()
**
** We distribute this method to all chat windows.
*/

/*************************************************************************/

static IPTR MM_ChannelRemove( struct IClass *cl, Object *obj, struct MP_APPLICATION_CHANNELREMOVE *msg )
{

//	  DoMethod(
	FORCHILD( obj, MUIA_Application_WindowList ) {
		if( MUIGetVar( child, MA_APPLICATION_CLASSID ) == CLASSID_WINDOWCHAT ) {
			DoMethod( child, MM_WINDOWCHAT_CHANNELREMOVE, msg->Channel );
		}
	} NEXTCHILD
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelChangeTopic()
**
** We distribute this method to all chat windows.
*/

/*************************************************************************/

static IPTR MM_ChannelChangeTopic( struct IClass *cl, Object *obj, struct MP_APPLICATION_CHANNELREMOVE *msg )
{

	FORCHILD( obj, MUIA_Application_WindowList ) {
		if( MUIGetVar( child, MA_APPLICATION_CLASSID ) == CLASSID_WINDOWCHAT ) {
			DoMethod( child, MM_WINDOWCHAT_CHANNELCHANGETOPIC, msg->Channel );
		}
	} NEXTCHILD
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelNickAdd()
**
*/

/*************************************************************************/

static IPTR MM_ChannelNickAdd( struct IClass *cl, Object *obj, struct MP_APPLICATION_CHANNELNICKADD *msg )
{

	FORCHILD( obj, MUIA_Application_WindowList ) {
		if( MUIGetVar( child, MA_APPLICATION_CLASSID ) == CLASSID_WINDOWCHAT ) {
			DoMethod( child, MM_WINDOWCHAT_CHANNELNICKADD, msg->Channel, msg->ChatNickEntry );
		}
	} NEXTCHILD

	return( 0 );
}
/* \\\ */
/* /// MM_ChannelNickRemove()
**
** We distribute this method to all chat windows.
*/

/*************************************************************************/

static IPTR MM_ChannelNickRemove( struct IClass *cl, Object *obj, struct MP_APPLICATION_CHANNELNICKREMOVE *msg )
{

	FORCHILD( obj, MUIA_Application_WindowList ) {
		if( MUIGetVar( child, MA_APPLICATION_CLASSID ) == CLASSID_WINDOWCHAT ) {
			DoMethod( child, MM_WINDOWCHAT_CHANNELNICKREMOVE, msg->Channel, msg->ChatNickEntry );
		}
	} NEXTCHILD

	return( 0 );
}
/* \\\ */

/* /// MM_VisualChange()
**
** We distribute this method to all chat windows.
*/

/*************************************************************************/

static IPTR MM_VisualChange( struct IClass *cl, Object *obj, Msg *msg )
{
	FORCHILD( obj, MUIA_Application_WindowList ) {
		if( MUIGetVar( child, MA_APPLICATION_CLASSID ) == CLASSID_WINDOWCHAT ) {
			DoMethod( child, MM_WINDOWCHAT_VISUALCHANGE );
		}
	} NEXTCHILD

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_Application_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_Application_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                 ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                         : return( OM_Dispose             ( cl, obj, (APTR) msg ) );

		case OM_GET                             : return( OM_Get                 ( cl, obj, (APTR) msg ) );

		case MUIM_Application_Load              : return( MM_Application_Load    ( cl, obj, (APTR) msg ) );
		case MUIM_Application_Save              : return( MM_Application_Load    ( cl, obj, (APTR) msg ) );

		case MM_APPLICATION_STARTUP             : return( MM_Startup             ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_WINDOWFIND          : return( MM_WindowFind          ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_WINDOWDISPOSE       : return( MM_WindowDispose       ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_WINDOWCHATNEW       : return( MM_WindowChatNew       ( cl, obj, (APTR) msg ) );

		case MM_APPLICATION_CHATSETACTIVE       : return( MM_ChatSetActive       ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_CHATREMOVE          : return( MM_ChatRemove          ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_CHATFINDACTIVE      : return( MM_ChatFindActive      ( cl, obj, (APTR) msg ) );

		case MM_APPLICATION_MESSAGERECEIVED     : return( MM_MessageReceived     ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_CHANNELADD          : return( MM_ChannelAdd          ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_CHANNELREMOVE       : return( MM_ChannelRemove       ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_CHANNELCHANGETOPIC  : return( MM_ChannelChangeTopic  ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_CHANNELNICKADD      : return( MM_ChannelNickAdd      ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_CHANNELNICKREMOVE   : return( MM_ChannelNickRemove   ( cl, obj, (APTR) msg ) );

		case MM_APPLICATION_VISUALCHANGE        : return( MM_VisualChange        ( cl, obj, (APTR) msg ) );


    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_Application_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_Application_InitClass( void )
{
	appclasses[ CLASSID_APPLICATION ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Application, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_Application_Dispatcher) );
	return( appclasses[ CLASSID_APPLICATION ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_Application_DisposeClass()
**
*/

/*************************************************************************/

void MCC_Application_DisposeClass( void )
{
	if( appclasses[ CLASSID_APPLICATION ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_APPLICATION ] );
		appclasses[ CLASSID_APPLICATION ] = NULL;
    }
}
/* \\\ */

