/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowquicksetup.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_network.h"
#include "muiclass_application.h"
#include "muiclass_windowquicksetup.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsserver.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_REALNAME = 0,
GID_USERNAME,
GID_NICKNAME,
GID_QUIT,
GID_CONTINUE,
GID_NETWORK,
WID_SETTINGS,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	memset( objs, 0, sizeof( objs ) );

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWQUICKSETUP_TITLE ),
			MUIA_Window_ID               , MAKE_ID('K','W','I','K'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
					Child, TextObject, MUIA_Text_PreParse, NULL,
										MUIA_Text_Contents, LGS( MSG_MUICLASS_WINDOWQUICKSETUP_INFO ),
										MUIA_InnerLeft, 0,
										MUIA_InnerRight, 0,
										End,
					Child, ColGroup(2),
						Child, MUICreateLabel( MSG_MUICLASS_WINDOWQUICKSETUP_REALNAME_GAD ),
						Child, objs[ GID_REALNAME     ] = MUICreateString( MSG_MUICLASS_WINDOWQUICKSETUP_REALNAME_GAD, SERVERENTRY_REALNAME_SIZEOF ),
						Child, MUICreateLabel( MSG_MUICLASS_WINDOWQUICKSETUP_USERNAME_GAD ),
						Child, objs[ GID_USERNAME     ] = MUICreateString( MSG_MUICLASS_WINDOWQUICKSETUP_USERNAME_GAD, SERVERENTRY_USERNAME_SIZEOF ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_WINDOWQUICKSETUP_NICKNAME_GAD ),
						Child, objs[ GID_NICKNAME     ] = MUICreateString( MSG_MUICLASS_WINDOWQUICKSETUP_NICKNAME_GAD, NICKENTRY_NICK_SIZEOF     ),
					End,
					Child, HGroup,
						Child, objs[ GID_CONTINUE     ] = MUICreateButton( MSG_MUICLASS_WINDOWQUICKSETUP_CONTINUE_GAD ),
						Child, HVSpace,
						Child, objs[ GID_QUIT         ] = MUICreateButton( MSG_MUICLASS_WINDOWQUICKSETUP_QUIT_GAD ),
					End,
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		SetAttrs( objs[ GID_REALNAME         ], MUIA_String_Reject, "/\?,:", TAG_DONE );
		SetAttrs( objs[ GID_USERNAME         ], MUIA_String_Reject, " /\?,:", TAG_DONE );

		DoMethod( obj                                      , MUIM_Notify, MUIA_Window_CloseRequest, TRUE , MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit );
		DoMethod( mccdata->mcc_ClassObjects[ GID_QUIT     ], MUIM_Notify, MUIA_Pressed            , FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit );
		DoMethod( mccdata->mcc_ClassObjects[ GID_CONTINUE ], MUIM_Notify, MUIA_Pressed            , FALSE, obj, 1, MM_WINDOWQUICKSETUP_CONTINUE );

		DoMethod( mccdata->mcc_ClassObjects[ GID_REALNAME ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_WINDOWQUICKSETUP_DISENABLE );
		DoMethod( mccdata->mcc_ClassObjects[ GID_USERNAME ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_WINDOWQUICKSETUP_DISENABLE );
		DoMethod( mccdata->mcc_ClassObjects[ GID_NICKNAME ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_WINDOWQUICKSETUP_DISENABLE );

		return( (IPTR) obj );
    }
	return( (IPTR) NULL );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static IPTR OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWQUICKSETUP ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */
/* /// MM_DisEnable()
**
*/

/*************************************************************************/

static IPTR MM_DisEnable( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
char *str;
ULONG count = 3;

	str = (char *) MUIGetVar( mccdata->mcc_ClassObjects[ GID_REALNAME ], MUIA_String_Contents );
	if( str && ( strlen( str ) > 3 ) ) {
		count--;
	}
	str = (char *) MUIGetVar( mccdata->mcc_ClassObjects[ GID_USERNAME ], MUIA_String_Contents );
	if( str && ( strlen( str ) > 3 ) ) {
		count--;
	}
	str = (char *) MUIGetVar( mccdata->mcc_ClassObjects[ GID_NICKNAME ], MUIA_String_Contents );
	if( str && ( strlen( str ) > 1 ) ) {
		count--;
	}
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CONTINUE ], MUIA_Disabled, (count ? TRUE : FALSE ), TAG_DONE );

	return( 1 );
}
/* \\\ */
/* /// MM_CheckSettings()
**
*/

/*************************************************************************/

static IPTR MM_CheckSettings( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
STRPTR str;
ULONG count = 3;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( !mccdata->mcc_ClassObjects[ WID_SETTINGS ] ) {
		mccdata->mcc_ClassObjects[ GID_NETWORK  ] = (Object*) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTNETWORK );
		mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object*) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );
	}

	str = (STRPTR) LocalReadConfig( OID_SVR_REALNAME );
	if( str && str[0] ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_REALNAME ], MUIA_String_Contents, str, TAG_DONE );
		debug("real name is there\n");
		count--;
	}
	str = (STRPTR) LocalReadConfig( OID_SVR_USERNAME );
	if( str && str[0] ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_USERNAME ], MUIA_String_Contents, str, TAG_DONE );
		debug("user name is there\n");
		count--;
	}

	{
		Object *serverlist;
		struct ServerEntry *se;

		serverlist = (Object*) LocalReadConfig( OID_SVR_LIST     );
		se = NULL;
		DoMethod( serverlist, MUIM_NList_GetEntry, 0, &se );
		if( se ) { /* we got a server, so no need to setup */
		debug("server name is there\n");
			count--;
		}
	}
	
	DoMethod( obj, MM_WINDOWQUICKSETUP_DISENABLE );
	
	if( count ) {
		SetAttrs( obj, MUIA_Window_Open, TRUE, TAG_DONE );
	}

	return( count );
}
/* \\\ */
/* /// MM_Continue()
**
*/

/*************************************************************************/

static IPTR MM_Continue( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
char *str;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	SetAttrs( obj, MUIA_Window_Open, FALSE, TAG_DONE );

	str = (char *) MUIGetVar( mccdata->mcc_ClassObjects[ GID_REALNAME ], MUIA_String_Contents );
	DoMethod( mccdata->mcc_ClassObjects[ WID_SETTINGS ], MM_WINDOWSETTINGS_WRITECONFIG, OID_SVR_REALNAME, str );
	str = (char *) MUIGetVar( mccdata->mcc_ClassObjects[ GID_USERNAME ], MUIA_String_Contents );
	DoMethod( mccdata->mcc_ClassObjects[ WID_SETTINGS ], MM_WINDOWSETTINGS_WRITECONFIG, OID_SVR_USERNAME, str );

	{
	ULONG n;
	Object *serverlist, *nicklist, *channellist;
	struct ServerEntry *se;
	struct NickEntry *ne;
	struct ChannelEntry *ce;
	char nickbuffer[ NICKENTRY_NICK_SIZEOF ];

		str = (char *) MUIGetVar( mccdata->mcc_ClassObjects[ GID_NICKNAME ], MUIA_String_Contents );

		serverlist   = (Object*) LocalReadConfig( OID_SVR_LIST     );
		nicklist     = (Object*) LocalReadConfig( OID_SVR_NICKLIST );
		channellist  = (Object*) LocalReadConfig( OID_SVR_CHANNELLIST );

		se = NULL;
		DoMethod( serverlist, MUIM_NList_GetEntry, 0, &se );
		if( !se ) { /* we got no server, so setup defaults */

			DoMethod( serverlist,
						MM_SERVERLIST_ADD,
						DEFAULT_SERVER_NAME,
						DEFAULT_SERVER_ADDRESS,
						DEFAULT_SERVER_PORT,
						DEFAULT_SERVER_PASSWORD,
						DEFAULT_SERVER_CHARSET,
						SERVERENTRYF_AUTOCONNECT );

			SetAttrs( serverlist, MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE ); /* make sure nicklist is valid */
			se = NULL;
			DoMethod( serverlist, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &se );
			if( se ) { /* if we get no server here, something is broken */
				/* add nick, nick_ and nick__ */
				ne = NULL;
				DoMethod( nicklist, MUIM_NList_GetEntry, 0, &ne );
				if( !ne ) { /* only add nicks if there are none */
					strncpy( nickbuffer, str, NICKENTRY_NICK_SIZEOF );
					for( n = 0 ; n < 3 ; n++ ) {
						if( ( ne = (APTR) DoMethod( nicklist, MM_NICKLIST_ADD, se, nickbuffer, "" ) ) ) {
							/* keep prefs in sync */
							DoMethod( nicklist, MUIM_NList_InsertSingle, ne, MUIV_NList_Insert_Bottom );
						}
						strcat( nickbuffer, "_" );
					}
				}
				/* add default channel */
				ce = NULL;
				DoMethod( channellist, MUIM_NList_GetEntry, 0, &ce );
				if( !ce ) { /* only add channels if there are none */
					if( ( ce = (APTR) DoMethod( channellist, MM_CHANNELLIST_ADD, se, DEFAULT_CHANNEL_NAME, DEFAULT_CHANNEL_PASSWORD ) ) ) {
						/* keep prefs in sync */
						DoMethod( channellist, MUIM_NList_InsertSingle, ce, MUIV_NList_Insert_Bottom );
					}
				}
			}
		}
	}
	DoMethod( mccdata->mcc_ClassObjects[ GID_NETWORK ], MM_NETWORK_SERVERCONNECTAUTO );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowQuickSetup_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowQuickSetup_Dispatcher)
{
	switch( msg->MethodID ) {
		case OM_NEW                             : return( OM_New             ( cl, obj, (APTR) msg ) );
		case OM_GET                             : return( OM_Get             ( cl, obj, (APTR) msg ) );

		case MM_WINDOWQUICKSETUP_CHECKSETTINGS  : return( MM_CheckSettings   ( cl, obj, (APTR) msg ) );
		case MM_WINDOWQUICKSETUP_DISENABLE      : return( MM_DisEnable       ( cl, obj, (APTR) msg ) );
		case MM_WINDOWQUICKSETUP_CONTINUE       : return( MM_Continue        ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowQuickSetup_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowQuickSetup_InitClass( void )
{
	appclasses[ CLASSID_WINDOWQUICKSETUP ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowQuickSetup_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWQUICKSETUP ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowQuickSetup_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowQuickSetup_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWQUICKSETUP ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWQUICKSETUP ] );
		appclasses[ CLASSID_WINDOWQUICKSETUP ] = NULL;
    }
}
/* \\\ */




