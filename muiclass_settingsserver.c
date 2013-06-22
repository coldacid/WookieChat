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

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_serverlist.h"
#include "muiclass_nicklist.h"
#include "muiclass_channellist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsserver.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_SERVERLIST = 0,
GID_SERVERADD,
GID_SERVERREMOVE,
GID_SERVERNAME,
GID_SERVERADDRESS,
GID_SERVERPORT,
GID_SERVERPASSWORD,
GID_SERVERAUTOCONNECT,
GID_SERVERCHARSET,
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
GID_REALNAME,
GID_USERNAME,
GID_RECONNECT,
GID_RECONNECTDELAY,
GID_DUPEDETECTION,
GID_LAST
};

/*
** configitem structure
*/

struct ConfigItem {
	ULONG GadgetID;
	ULONG ObjectID;
	ULONG Attr;
	LONG  Default;
};

static struct ConfigItem TAB_CONFIGITEMS[] = {
	{ GID_REALNAME          , OID_SVR_REALNAME         , MUIA_String_Contents, (LONG) "" },
	{ GID_USERNAME          , OID_SVR_USERNAME         , MUIA_String_Contents, (LONG) "" },
	{ GID_RECONNECT         , OID_SVR_RECONNECT        , MUIA_String_Integer , (LONG) 7 },
	{ GID_RECONNECTDELAY    , OID_SVR_RECONNECTDELAY   , MUIA_String_Integer , (LONG) 30 },
	{ GID_DUPEDETECTION     , OID_SVR_DUPEDETECTION    , MUIA_Selected       , (LONG) 0 },
	{ -1,0,0,0 },
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
static STRPTR TAB_GROUP_HIGHLIGHTMODES[ MSG_PG_ADDITIONAL - MSG_PG_CONNECTIONS + 2 ];

	MUIInitStringArray( TAB_GROUP_HIGHLIGHTMODES, MSG_PG_CONNECTIONS, MSG_PG_ADDITIONAL );

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Register_Titles, TAB_GROUP_HIGHLIGHTMODES,
				Child, VGroup,
					Child, HGroup,
						Child, NListviewObject, MUIA_NListview_NList, objs[ GID_SERVERLIST ] = ServerListObject, End, End,
						Child, VGroup,
							Child, NListviewObject, MUIA_NListview_NList, objs[ GID_NICKLIST    ] = NickListObject, End, End,
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

							Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHANNELLIST ] = ChannelListObject, End, End,
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
						Child, objs[ GID_SERVERADD         ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_ADD_GAD ),
						Child, objs[ GID_SERVERREMOVE      ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSSERVER_REMOVE_GAD ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_NAME_GAD ),
						Child, objs[ GID_SERVERNAME        ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSSERVER_NAME_GAD, SERVERENTRY_NAME_SIZEOF ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_ADDRESS_GAD ),
						Child, objs[ GID_SERVERADDRESS     ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_ADDRESS_GAD, SERVERENTRY_ADDRESS_SIZEOF ),
					End,
					Child, HGroup,
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_PORT_GAD ),
						Child, objs[ GID_SERVERPORT        ] = MUICreateInteger( MSG_MUICLASS_SETTINGSSERVER_PORT_GAD, 5 ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_PASSWORD_GAD ),
						Child, objs[ GID_SERVERPASSWORD    ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_PASSWORD_GAD, SERVERENTRY_PASSWORD_SIZEOF ),
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_CHARSET_GAD ),
						Child, objs[ GID_SERVERCHARSET     ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_CHARSET_GAD, SERVERENTRY_CHARSET_SIZEOF ),
						Child, objs[ GID_SERVERAUTOCONNECT ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSSERVER_AUTOCONNECT_GAD, FALSE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSERVER_AUTOCONNECT_GAD ),
						Child, HVSpace,
					End,
				End,
				Child, ColGroup(2), 
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_REALNAME_GAD ),
					Child, objs[ GID_REALNAME              ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_REALNAME_GAD, SERVERENTRY_REALNAME_SIZEOF ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_USERNAME_GAD ),
					Child, objs[ GID_USERNAME              ] = MUICreateString( MSG_MUICLASS_SETTINGSSERVER_USERNAME_GAD, SERVERENTRY_USERNAME_SIZEOF ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_RECONNECT_GAD ),
					Child, HGroup,
						Child, objs[ GID_RECONNECT         ] = MUICreateInteger( MSG_MUICLASS_SETTINGSSERVER_RECONNECT_GAD, 5 ),
						Child, HVSpace,
					End,
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSSERVER_RECONNECTDELAY_GAD ),
					Child, HGroup,
						Child, objs[ GID_RECONNECTDELAY    ] = MUICreateInteger( MSG_MUICLASS_SETTINGSSERVER_RECONNECTDELAY_GAD, 5 ),
						Child, HVSpace,
					End,
					Child, HGroup,
						Child, objs[ GID_DUPEDETECTION     ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSSERVER_DUPEDETECTION_GAD, FALSE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSERVER_DUPEDETECTION_GAD ),
						Child, HVSpace,
					End,
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));
		
		SetAttrs( objs[ GID_SERVERADDRESS    ], MUIA_String_Reject, " /\?,:", TAG_DONE );
		SetAttrs( objs[ GID_SERVERPASSWORD   ], MUIA_String_Reject, " /\?,:", TAG_DONE );
		SetAttrs( objs[ GID_SERVERCHARSET    ], MUIA_String_Reject, " /\?,:", TAG_DONE );
		SetAttrs( objs[ GID_NICKNAME         ], MUIA_String_Reject, " /\?,:", TAG_DONE );
		SetAttrs( objs[ GID_NICKPASSWORD     ], MUIA_String_Reject, " /\?,:", TAG_DONE );
		SetAttrs( objs[ GID_CHANNELNAME      ], MUIA_String_Reject, " /\?,:\x7", TAG_DONE );
		SetAttrs( objs[ GID_CHANNELPASSWORD  ], MUIA_String_Reject, " /\?,:", TAG_DONE );
		SetAttrs( objs[ GID_SERVERLIST       ] , MA_SERVERLIST_NICKLISTOBJ, objs[ GID_NICKLIST ], MA_SERVERLIST_CHANNELLISTOBJ, objs[ GID_CHANNELLIST ], TAG_DONE );

		SetAttrs( objs[ GID_REALNAME         ], MUIA_String_Reject, "/\?,:", TAG_DONE );
		SetAttrs( objs[ GID_USERNAME         ], MUIA_String_Reject, " /\?,:", TAG_DONE );


		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_SERVERLIST        ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_SERVERLISTTOGADGETS );
		DoMethod( objs[ GID_SERVERADD         ], MUIM_Notify, MUIA_Pressed        , FALSE         , objs[ GID_SERVERLIST ], 6, MM_SERVERLIST_ADD, NULL, NULL, 6667, NULL, NULL, 0 );
		DoMethod( objs[ GID_SERVERADD         ], MUIM_Notify, MUIA_Pressed        , FALSE         , objs[ GID_SERVERLIST ], 3, MUIM_Set, MUIA_NList_Active, MUIV_NList_Active_Bottom );
		DoMethod( objs[ GID_SERVERREMOVE      ], MUIM_Notify, MUIA_Pressed        , FALSE         , objs[ GID_SERVERLIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );
		DoMethod( objs[ GID_SERVERNAME        ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_SERVERGADGETSTOLIST );
		DoMethod( objs[ GID_SERVERADDRESS     ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_SERVERGADGETSTOLIST );
		DoMethod( objs[ GID_SERVERPORT        ], MUIM_Notify, MUIA_String_Integer , MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_SERVERGADGETSTOLIST );
		DoMethod( objs[ GID_SERVERPASSWORD    ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_SERVERGADGETSTOLIST );
		DoMethod( objs[ GID_SERVERAUTOCONNECT ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_SERVERGADGETSTOLIST );

		DoMethod( objs[ GID_NICKLIST          ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_NICKLISTTOGADGETS );
		DoMethod( objs[ GID_NICKADD           ], MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MM_SETTINGSSERVER_NICKADD    );
		DoMethod( objs[ GID_NICKREMOVE        ], MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MM_SETTINGSSERVER_NICKREMOVE );
		DoMethod( objs[ GID_NICKNAME          ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_NICKGADGETSTOLIST );
		DoMethod( objs[ GID_NICKPASSWORD      ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_NICKGADGETSTOLIST );

		DoMethod( objs[ GID_CHANNELLIST       ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_CHANNELLISTTOGADGETS );
		DoMethod( objs[ GID_CHANNELADD        ], MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MM_SETTINGSSERVER_CHANNELADD    );
		DoMethod( objs[ GID_CHANNELREMOVE     ], MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MM_SETTINGSSERVER_CHANNELREMOVE );
		DoMethod( objs[ GID_CHANNELNAME       ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_CHANNELGADGETSTOLIST );
		DoMethod( objs[ GID_CHANNELPASSWORD   ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSSERVER_CHANNELGADGETSTOLIST );

		DoMethod( obj, MM_SETTINGSSERVER_RESETTODEFAULTS );
	    DoMethod( obj, MM_SETTINGSSERVER_SERVERLISTTOGADGETS );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// MM_ResetToDefaults()
**
*/

/*************************************************************************/

static ULONG MM_ResetToDefaults( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr, TAB_CONFIGITEMS[ i ].Default, MUIA_ObjectID, TAB_CONFIGITEMS[ i ].ObjectID, TAG_DONE );
	}
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
ULONG i;

	if( msg->ObjectID == OID_SVR_LIST ) {
		return( (IPTR) mccdata->mcc_ClassObjects[ GID_SERVERLIST ] );
	}
	if( msg->ObjectID == OID_SVR_NICKLIST ) {
		return( (IPTR) mccdata->mcc_ClassObjects[ GID_NICKLIST ] );
	}
	if( msg->ObjectID == OID_SVR_CHANNELLIST ) {
		return( (IPTR) mccdata->mcc_ClassObjects[ GID_CHANNELLIST ] );
	}

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].ObjectID == msg->ObjectID ) {
			return( (ULONG) MUIGetVar( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr ) );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_WriteConfig()
**
*/

/*************************************************************************/

static ULONG MM_WriteConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSSERVER_WRITECONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].ObjectID == msg->ObjectID ) {
			SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr, msg->Data, TAG_DONE );
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_DisEnable()
**
*/

/*************************************************************************/

static ULONG MM_DisEnable( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG pos;
BOOL server, nick, channel;

	server = nick = channel = FALSE;

	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIA_NList_Active );
	if( pos < 0 ) {
		server = TRUE;
	}
	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIA_NList_Active );
	if( pos < 0 ) {
		nick = TRUE;
	}
	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIA_NList_Active );
	if( pos < 0 ) {
		channel = TRUE;
	}

	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERREMOVE      ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERNAME        ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERADDRESS     ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERPORT        ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERPASSWORD    ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERCHARSET     ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERAUTOCONNECT ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKLIST          ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKADD           ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKREMOVE        ], MUIA_Disabled, server | nick   , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKNAME          ], MUIA_Disabled, server | nick   , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKPASSWORD      ], MUIA_Disabled, server | nick   , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELLIST       ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELADD        ], MUIA_Disabled, server          , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELREMOVE     ], MUIA_Disabled, server | channel, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELNAME       ], MUIA_Disabled, server | channel, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELPASSWORD   ], MUIA_Disabled, server | channel, TAG_DONE );

	return( 0 );
}
/* \\\ */
/* /// MM_ServerGadgetsToList()
**
*/

/*************************************************************************/

static ULONG MM_ServerGadgetsToList( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &se );

	if( se ) {
		STRPTR str;
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERNAME     ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) se->se_Name, (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERADDRESS  ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) se->se_Address, (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERPASSWORD ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) se->se_Password, (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERCHARSET ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) se->se_Charset, (char *) str );
		}
		se->se_Port  =   MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERPORT        ], MUIA_String_Integer );
		se->se_Flags = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_SERVERAUTOCONNECT ], MUIA_Selected ) ? SERVERENTRYF_AUTOCONNECT : 0 );

		DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ServerListToGadgets()
**
*/

/*************************************************************************/

static ULONG MM_ServerListToGadgets( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &se );

	if( se ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERNAME        ], MUIA_NoNotify, TRUE, MUIA_String_Contents, se->se_Name    , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERADDRESS     ], MUIA_NoNotify, TRUE, MUIA_String_Contents, se->se_Address , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERPASSWORD    ], MUIA_NoNotify, TRUE, MUIA_String_Contents, se->se_Password, TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERCHARSET     ], MUIA_NoNotify, TRUE, MUIA_String_Contents, se->se_Charset , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERPORT        ], MUIA_NoNotify, TRUE, MUIA_String_Integer , se->se_Port    , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SERVERAUTOCONNECT ], MUIA_NoNotify, TRUE, MUIA_Selected       , ( ( se->se_Flags & SERVERENTRYF_AUTOCONNECT ) ? TRUE : FALSE ), TAG_DONE );
	}
	DoMethod( obj, MM_SETTINGSSERVER_DISENABLE );

	return( 0 );
}

/* \\\ */

/* /// MM_NickAdd()
**
*/

/*************************************************************************/

static ULONG MM_NickAdd( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se = NULL;
struct NickEntry *ne;

	DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &se );

	if( se ) {
		if( ( ne = (APTR) DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MM_NICKLIST_ADD, se, NULL, NULL ) ) ) {
			DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_InsertSingle, ne, MUIV_NList_Insert_Bottom );
			SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
			SetAttrs( _win( mccdata->mcc_ClassObjects[ GID_NICKLIST ] ), MUIA_Window_ActiveObject, mccdata->mcc_ClassObjects[ GID_NICKNAME ], TAG_DONE );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_NickRemove()
**
*/

/*************************************************************************/

static ULONG MM_NickRemove( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG pos;
struct NickEntry *ne = NULL;

	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIA_NList_Active );
	if( pos >= 0 ) {
		DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_GetEntry, pos, &ne );
		DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_Remove, pos );
		DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MM_NICKLIST_REMOVE, ne );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_Set, MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_NickGadgetsToList()
**
*/

/*************************************************************************/

static ULONG MM_NickGadgetsToList( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct NickEntry *ne = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ne );

	if( ne ) {
		STRPTR str;
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_NICKNAME       ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ne->ne_Name    , (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_NICKPASSWORD ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ne->ne_Password, (char *) str );
		}
		DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_NickListToGadgets()
**
*/

/*************************************************************************/

static ULONG MM_NickListToGadgets( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct NickEntry *ne = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ne );

	if( ne ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKNAME     ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ne->ne_Name    , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKPASSWORD ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ne->ne_Password, TAG_DONE );
	}
	DoMethod( obj, MM_SETTINGSSERVER_DISENABLE );

	return( 0 );
}

/* \\\ */

/* /// MM_ChannelAdd()
**
*/

/*************************************************************************/

static ULONG MM_ChannelAdd( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se = NULL;
struct ChannelEntry *ce;

	DoMethod( mccdata->mcc_ClassObjects[ GID_SERVERLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &se );

	if( se ) {
		if( ( ce = (APTR) DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MM_CHANNELLIST_ADD, se, NULL, NULL ) ) ) {
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_InsertSingle, ce, MUIV_NList_Insert_Bottom );
			SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
			SetAttrs( _win( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ] ), MUIA_Window_ActiveObject, mccdata->mcc_ClassObjects[ GID_CHANNELNAME ], TAG_DONE );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelRemove()
**
*/

/*************************************************************************/

static ULONG MM_ChannelRemove( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG pos;
struct ChannelEntry *ce = NULL;

	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIA_NList_Active );
	if( pos >= 0 ) {
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_GetEntry, pos, &ce );
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_Remove, pos );
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MM_CHANNELLIST_REMOVE, ce );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_Set, MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelGadgetsToList()
**
*/

/*************************************************************************/

static ULONG MM_ChannelGadgetsToList( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChannelEntry *ce = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ce );

	if( ce ) {
		STRPTR str;
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_CHANNELNAME     ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ce->ce_Name    , (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_CHANNELPASSWORD ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ce->ce_Password, (char *) str );
		}
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelListToGadgets()
**
*/

/*************************************************************************/

static ULONG MM_ChannelListToGadgets( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChannelEntry *ce = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ce );

	if( ce ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELNAME     ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ce->ce_Name    , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELPASSWORD ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ce->ce_Password, TAG_DONE );
	}
	DoMethod( obj, MM_SETTINGSSERVER_DISENABLE );

	return( 0 );
}

/* \\\ */

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
		case OM_NEW                                     : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_READCONFIG               : return( MM_ReadConfig              ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_WRITECONFIG              : return( MM_WriteConfig              ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_RESETTODEFAULTS          : return( MM_ResetToDefaults         ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_DISENABLE                : return( MM_DisEnable               ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_SERVERGADGETSTOLIST      : return( MM_ServerGadgetsToList     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_SERVERLISTTOGADGETS      : return( MM_ServerListToGadgets     ( cl, obj, (APTR) msg ) );

		case MM_SETTINGSSERVER_NICKADD                  : return( MM_NickAdd                 ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_NICKREMOVE               : return( MM_NickRemove              ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_NICKGADGETSTOLIST        : return( MM_NickGadgetsToList       ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_NICKLISTTOGADGETS        : return( MM_NickListToGadgets       ( cl, obj, (APTR) msg ) );

		case MM_SETTINGSSERVER_CHANNELADD               : return( MM_ChannelAdd              ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_CHANNELREMOVE            : return( MM_ChannelRemove           ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_CHANNELGADGETSTOLIST     : return( MM_ChannelGadgetsToList    ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSERVER_CHANNELLISTTOGADGETS     : return( MM_ChannelListToGadgets    ( cl, obj, (APTR) msg ) );
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
	appclasses[ CLASSID_SETTINGSSERVER ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Register, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsServer_Dispatcher) );
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



