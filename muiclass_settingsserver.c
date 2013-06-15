/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingsserver.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>
#include <string.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_serverlist.h"
#include "muiclass_nicklist.h"
#include "muiclass_channellist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsserver.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_SERVERLIST = 0,
GID_NICKLIST,
GID_NICKADD,
GID_NICKREMOVE,
GID_NICKNAME,
GID_NICKPASSWORD,
GID_CHANNELLIST,
GID_CHANNELADD,
GID_CHANNELREMOVE,
GID_CHANNELNAME,
GID_CHANNELPASSWORD,
GID_ADDRESS,
GID_PORT,
GID_ADD,
GID_REMOVE,
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

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, FALSE, MUIA_Group_PageMode, TRUE,

				Child, HGroup,
					Child, NListviewObject, MUIA_NListview_NList, objs[ GID_SERVERLIST ] = ServerListObject, End, End,
					Child, VGroup,
						Child, objs[ GID_NICKLIST    ] = NickListObject, End,
						Child, HGroup,
							Child, objs[ GID_NICKADD      ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_NICKADD_GAD ),
							Child, objs[ GID_NICKREMOVE   ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_NICKREMOVE_GAD ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSERVER_NICKNAME_GAD ),
							Child, objs[ GID_NICKNAME     ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_NICKNAME_GAD, NICKENTRY_NICK_SIZEOF     ),
						End,
						Child, HGroup,
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSERVER_NICKPASSWORD_GAD ),
							Child, objs[ GID_NICKPASSWORD ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_NICKPASSWORD_GAD, NICKENTRY_PASSWORD_SIZEOF ),
						End,

						Child, objs[ GID_CHANNELLIST ] = ChannelListObject, End,
						Child, HGroup,
							Child, objs[ GID_CHANNELADD      ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_CHANNELADD_GAD ),
							Child, objs[ GID_CHANNELREMOVE   ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_CHANNELREMOVE_GAD ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSERVER_CHANNELNAME_GAD ),
							Child, objs[ GID_CHANNELNAME     ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_CHANNELNAME_GAD, CHANNELENTRY_CHANNEL_SIZEOF     ),
						End,
						Child, HGroup,
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSERVER_CHANNELPASSWORD_GAD ),
							Child, objs[ GID_CHANNELPASSWORD ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_CHANNELPASSWORD_GAD, CHANNELENTRY_PASSWORD_SIZEOF ),
						End,
					End,
				End,
				Child, HGroup,
					Child, objs[ GID_ADD        ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_ADD_GAD ),
					Child, objs[ GID_REMOVE     ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_REMOVE_GAD ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_ADDRESS_GAD ),
					Child, objs[ GID_ADDRESS    ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSSERVER_ADDRESS_GAD, SERVERENTRY_ADDRESS_SIZEOF ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_PORT_GAD ),
					Child, objs[ GID_PORT       ] = MUICreateInteger( MSG_MUICLASS_SETTINGSSERVER_PORT_GAD, 5 ),
					Child, HVSpace,
				End,

		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));
		
		SetAttrs( objs[ GID_ADDRESS  ], MUIA_String_Reject, " /\?", TAG_DONE );
		SetAttrs( objs[ GID_SERVERLIST ] , MA_SERVERLIST_NICKLISTOBJ, objs[ GID_NICKLIST ], MA_SERVERLIST_CHANNELLISTOBJ, objs[ GID_CHANNELLIST ], TAG_DONE );

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

//		  DoMethod( objs[ GID_ADD       ], MUIM_Notify, MUIA_Pressed, FALSE, objs[ GID_SERVERLIST ], 3, MM_SERVERLIST_ADD, 0, 0 );
//		  DoMethod( objs[ GID_REMOVE    ], MUIM_Notify, MUIA_Pressed, FALSE, objs[ GID_SERVERLIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );

//		  DoMethod( objs[ GID_SERVERLIST ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_LISTTOGADGETS );
//		  DoMethod( objs[ GID_SERVER     ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_GADGETSTOLIST );
//		  DoMethod( objs[ GID_TEXT      ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_GADGETSTOLIST );

//		  DoMethod( obj, MM_SETTINGSSERVER_LISTTOGADGETS );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
#if 0
/* /// MM_DisEnable()
**
*/

/*************************************************************************/

static ULONG MM_DisEnable( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG pos;
BOOL disabled = TRUE;

	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIA_NList_Active );
	if( pos >= 0 ) {
		disabled = FALSE;
	}

	SetAttrs( mccdata->mcc_ClassObjects[ GID_REMOVE  ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVER   ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_TEXT    ], MUIA_Disabled, disabled, TAG_DONE );

	return( 0 );
}
/* \\\ */
/* /// MM_GadgetsToList()
**
*/

/*************************************************************************/

static ULONG MM_GadgetsToList( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *ae = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ae );

	if( ae ) {
		STRPTR str;
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVER ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ae->ae_Server, (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_TEXT ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ae->ae_Text, (char *) str );
		}
		DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ListToGadgets()
**
*/

/*************************************************************************/

static ULONG MM_ListToGadgets( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *ae = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ae );

	if( ae ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVER ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ae->ae_Server, TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_TEXT  ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ae->ae_Text , TAG_DONE );
	}

	DoMethod( obj, MM_SETTINGSSERVER_DISENABLE );

	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSSERVER_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	switch( msg->ObjectID ) {
		case OID_ALI_LIST:
			return( (ULONG) mccdata->mcc_ClassObjects[ GID_SERVERLIST ] );
		default:
			return( 0 );
	}
}
/* \\\ */
#endif
/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsServer_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsServer_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                     ( cl, obj, (APTR) msg ) );
//		  case MM_SETTINGSSERVER_DISENABLE         : return( MM_DisEnable                     ( cl, obj, (APTR) msg ) );
//		  case MM_SETTINGSSERVER_GADGETSTOLIST     : return( MM_GadgetsToList                 ( cl, obj, (APTR) msg ) );
//		  case MM_SETTINGSSERVER_LISTTOGADGETS     : return( MM_ListToGadgets                 ( cl, obj, (APTR) msg ) );
//		  case MM_SETTINGSSERVER_READCONFIG        : return( MM_ReadConfig  ( cl, obj, (APTR) msg ) );

	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsServer_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsServer_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSSERVER ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsServer_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSSERVER ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsServer_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsServer_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSSERVER ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSSERVER ] );
		appclasses[ CLASSID_SETTINGSSERVER ] = NULL;
    }
}
/* \\\ */



