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

#include "intern.h"
#include "locale.h"
#include "functions.h"
#include "muiclass.h"
#include "muiclass_audio.h"
#include "muiclass_application.h"
#include "muiclass_network.h"
#include "muiclass_windowchat.h"
#include "muiclass_windowquit.h"
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
GID_AUDIO,
GID_NETWORK,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	struct DiskObject     *mcc_DiskObject;
	IPTR                   mcc_ReadargsArray[ READARGS_LAST ];

};

/*************************************************************************/

/* /// OM_New()
**
*/


static STRPTR classlist[] = { (STRPTR)"NList.mcc", (STRPTR)"NListview.mcc", (STRPTR)"NListtree.mcc", (STRPTR)"BetterString.mcc", NULL };

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	if( ( obj = (Object *)DoSuperNew( cl, obj,
			        MUIA_Application_UsedClasses, classlist,
					MUIA_Application_Author     , (char*) AUTHORNAME,
					MUIA_Application_Base       , (char*) APPLICATIONNAME "_NEW",
					MUIA_Application_Title      , (char*) APPLICATIONNAME "_NEW",
					MUIA_Application_Version    , (char*) VERSION_MUI,
					MUIA_Application_Copyright  , (char*) VERSION_COPYRIGHT,
					MUIA_Application_Description, (char*) LGS( MSG_MUICLASS_APPLICATION_DESCRIPTION ),
					MUIA_Application_Window     , objs[ WID_QUIT          ] = WindowQuitObject      , End,
					MUIA_Application_Window     , objs[ WID_ABOUT         ] = WindowAboutObject     , End,
					MUIA_Application_Window     , objs[ WID_SETTINGS      ] = WindowSettingsObject  , End,
					MUIA_Application_Window     , objs[ WID_IGNORELIST    ] = WindowIgnoreListObject, End,
					MUIA_Application_Window     , objs[ WID_URLGRABBER    ] = WindowURLGrabberObject, End,
					/* this is just a dummy to store/handle our non visible classes, without additional code */
					MUIA_Application_Window     , WindowObject, MA_APPLICATION_CLASSID, -1,
									WindowContents, VGroup,
										Child   , objs[ GID_AUDIO   ] = AudioObject , End,
										Child   , objs[ GID_NETWORK ] = NetworkObject , End,
									End,
						End,
					TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		SetAttrs( objs[ GID_NETWORK ], MA_NETWORK_OBJECTSETTINGS, objs[ WID_SETTINGS ], MA_NETWORK_OBJECTAUDIO, objs[ GID_AUDIO    ], TAG_DONE );
		SetAttrs( objs[ GID_AUDIO   ], MA_AUDIO_OBJECTSETTINGS  , objs[ WID_SETTINGS ], TAG_DONE );

		/* load settings */
		DoMethod( obj, MUIM_Application_Load, MUIV_Application_Load_ENVARC );
		DoMethod( obj, MUIM_Application_Load, MUIV_Application_Load_ENV );
		
		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

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

static ULONG OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_OBJECTNETWORK       : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ GID_NETWORK       ] ; return( TRUE );
		case MA_APPLICATION_WINDOWQUIT          : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_QUIT          ] ; return( TRUE );
		case MA_APPLICATION_WINDOWABOUT         : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_ABOUT         ] ; return( TRUE );
		case MA_APPLICATION_WINDOWSETTINGS      : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_SETTINGS      ] ; return( TRUE );
		case MA_APPLICATION_WINDOWIGNORELIST    : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_IGNORELIST    ] ; return( TRUE );
		case MA_APPLICATION_WINDOWURLGRABBER    : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_URLGRABBER    ] ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */

/* /// MM_Startup()
**
*/

/*************************************************************************/

static ULONG MM_Startup( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	mccdata->mcc_DiskObject	= Icon_GetPutDiskObject( NULL );
	if( mccdata->mcc_DiskObject ) {
		SetAttrs( obj, MUIA_Application_DiskObject, mccdata->mcc_DiskObject, TAG_DONE );
	}

	if( !wbmessage ) {
		struct RDArgs *readargs;
		if( !( readargs = ReadArgs( (CONST_STRPTR) READARGS_TEMPLATE, (LONG*) mccdata->mcc_ReadargsArray, NULL ) ) ) {
			Dos_ShowFailure();
		} else {
			/* insert arg handling */
			if( readargs ) {
				FreeArgs( readargs );
			}
		}
	}

	/* auto connect or open prefs */
	DoMethod( mccdata->mcc_ClassObjects[ GID_NETWORK ], MM_NETWORK_AUTORECONNECT );

#if ENABLE_NEWWOOKIECODE
//	  SetAttrs( mccdata->mcc_ClassObjects[ WID_CHAT ], MUIA_Window_Open, TRUE, TAG_DONE );
//	  DoMethod( mccdata->mcc_ClassObjects[ WID_CHAT ], MM_WINDOWCHAT_COLORCHANGE );
#endif

	return( 0 );
}
/* \\\ */
/* /// MM_Application_Load()
**
*/

/*************************************************************************/

static ULONG MM_Application_Load( struct IClass *cl, Object *obj, Msg msg )
{
ULONG rc = DoSuperMethodA( cl, obj, (Msg) msg );
//struct mccdata *mccdata = INST_DATA( cl, obj );

//	  DoMethod( mccdata->mcc_ClassObjects[ WID_CHAT ], MM_WINDOWCHAT_COLORCHANGE );

	return( rc );
}
/* \\\ */
/* /// MM_WindowDispose()
**
*/

/*************************************************************************/

static ULONG MM_WindowDispose( struct IClass *cl UNUSED, Object *obj, struct MP_APPLICATION_WINDOWDISPOSE *msg )
{
Object *win;

	if( ( win = (Object *) DoMethod( obj, MM_APPLICATION_WINDOWFIND, msg->Window ) ) ) {
		DoMethod( _app(obj), OM_REMMEMBER, win );
		MUI_DisposeObject( win );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_WindowNewChat()
**
*/

/*************************************************************************/

static ULONG MM_WindowNewChat( struct IClass *cl UNUSED, Object *obj, Msg *msg )
{
Object *win;

	if( ( win = WindowChatObject, End ) ) {
		DoMethod( obj, OM_ADDMEMBER, win );
		if( win ) {
			SetAttrs( win, MUIA_Window_Open, TRUE, TAG_DONE );
		}
	}
	return( (ULONG) win );
}
/* \\\ */
/* /// MM_ChatFind()
**
*/

/*************************************************************************/

static ULONG MM_WindowFind( struct IClass *cl UNUSED , Object *obj, struct MP_APPLICATION_WINDOWFIND *msg )
{
	FORCHILD( obj, MUIA_Application_WindowList ) {
		if( child == msg->Window ) {
			return( (ULONG) child );
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
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );

		case OM_GET                          : return( OM_Get                           ( cl, obj, (APTR) msg ) );

		case MUIM_Application_Load           : return( MM_Application_Load              ( cl, obj, (APTR) msg ) );
		case MUIM_Application_Save           : return( MM_Application_Load              ( cl, obj, (APTR) msg ) );

		case MM_APPLICATION_STARTUP          : return( MM_Startup                       ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_WINDOWFIND       : return( MM_WindowFind                    ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_WINDOWDISPOSE    : return( MM_WindowDispose                 ( cl, obj, (APTR) msg ) );
		case MM_APPLICATION_WINDOWNEWCHAT    : return( MM_WindowNewChat                 ( cl, obj, (APTR) msg ) );
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




