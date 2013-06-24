/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowchat.c
*/

#include <libraries/mui.h>
#include <libraries/gadtools.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>
#include <string.h>
#include <stdio.h>

#include "system.h"
#include "functions.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_network.h"
#include "muiclass_settingscolor.h"
#include "muiclass_application.h"
#include "muiclass_windowchat.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowsettings.h"
#include "muiclass_chatuserlist.h"
#include "muiclass_chatchannellist.h"
#include "muiclass_chatlog.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_MENUSTRIP = 0,
GID_NETWORK,
WID_SETTINGS,
MID_SELECTSERVER,
MID_TABNEW,
MID_TABNEWGLOBAL,
MID_TABCLOSE,
MID_HIDE,
MID_HISTORYCLEAR,
MID_HISTORYCLEARALL,
MID_HISTORYSAVE,
MID_ABOUT,
MID_QUIT,
MID_CUT,
MID_COPY,
MID_PASTE,
MID_SETTINGS,
MID_SETTINGSMUI,
MID_MULTICOLUMNDISPLAY,
MID_CLIPBOARD,
MID_MUTESOUND,
MID_SETTINGSSAVE,
MID_DCCINCOMING,
MID_DCCOUTGOIING,
MID_URLGRABBER,
MID_IGNORELIST,
GID_CLOSETAB,
GID_TOPIC,
GID_MODET,
GID_MODEN,
GID_MODES,
GID_MODEI,
GID_MODEP,
GID_MODEM,
GID_MODEB,
GID_MODEK,
GID_MODEKEYWORD,
GID_MODEL,
GID_MODELIMIT,
GID_CHATUSERLIST,
GID_CHATCHANNELLIST,
GID_CHATLOG,
GID_CHATMESSAGE,
GID_LAST
};

#define WINDOWTITLE_SIZEOF 0x100
#define TOPIC_SIZEOF 0x400
#define KEYWORDLIMIT_SIZEOF 40
#define USERLIMIT_SIZEOF    40
#define MESSAGE_SIZEOF      490

#define FIRSTMENU_ITEM MID_SELECTSERVER	
#define LASTMENU_ITEM MID_IGNORELIST
 
/*
** data used by this class
*/
#define COMMAND_SIZEOF 0x1000

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	char                   mcc_WindowTitle[ WINDOWTITLE_SIZEOF + 2 ];
	char                   mcc_CommandBuffer[ COMMAND_SIZEOF ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWCHAT_TITLE ),
			MUIA_Window_ID               , MAKE_ID('C','H','A','T'),
			MUIA_Window_Menustrip, MenustripObject,
								Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_PROJECT_TITLE ),
								Child, objs[ MID_SELECTSERVER       ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SELECTSERVER_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_SELECTSERVER_KEY ), End,
								Child, objs[ MID_TABNEW             ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_NEWSERVERTAB_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_NEWSERVERTAB_KEY ), End,
								Child, objs[ MID_TABNEWGLOBAL       ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_NEWGLOBALQUERYTAB_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_NEWGLOBALQUERYTAB_KEY ), End,
								Child, objs[ MID_TABCLOSE           ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_CLOSESERVERTAB_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_CLOSESERVERTAB_KEY ), End,
								Child, MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
								Child, objs[ MID_HIDE               ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HIDE_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_HIDE_KEY ), End,
								Child, MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
								Child, objs[ MID_HISTORYCLEAR       ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYCLEAR_ITEM ), End,
								Child, objs[ MID_HISTORYCLEARALL    ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYCLEARALL_ITEM ), End,
								Child, objs[ MID_HISTORYSAVE        ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYSAVE_ITEM ), End,
								Child, MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
								Child, objs[ MID_ABOUT              ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_ABOUT_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_ABOUT_KEY ), End,
								Child, objs[ MID_QUIT               ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_QUIT_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_QUIT_KEY ), End,
							End,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_EDIT_TITLE ),
								Child, objs[ MID_CUT                ] = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_CUT_ITEM )  , MUIA_Menuitem_Shortcut, LGS( MSG_MENU_CUT_KEY )  , End,
								Child, objs[ MID_COPY               ] = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_COPY_ITEM ) , MUIA_Menuitem_Shortcut, LGS( MSG_MENU_COPY_KEY ) , End,
								Child, objs[ MID_PASTE              ] = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_PASTE_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_PASTE_KEY ), End,
							End,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_SETTINGS_TITLE ),
								Child, objs[ MID_SETTINGS           ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSMAIN_ITEM ), End,
								Child, objs[ MID_SETTINGSMUI        ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSMUI_ITEM ), End,
								Child, objs[ MID_MULTICOLUMNDISPLAY ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_MULTICOLUMNCHATDISPLAY_ITEM ), MUIA_Menuitem_Checkit, TRUE, MUIA_Menuitem_Toggle, TRUE, End,
								Child, objs[ MID_CLIPBOARD          ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_COLUMNMARKINGCLIPBOARD_ITEM ), MUIA_Menuitem_Checkit, TRUE, MUIA_Menuitem_Toggle, TRUE, End,
								Child, objs[ MID_MUTESOUND          ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_MUTEALLSOUNDS_ITEM ), MUIA_Menuitem_Checkit, TRUE, MUIA_Menuitem_Toggle, TRUE, End,
								Child, objs[ MID_SETTINGSSAVE       ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSSAVE_ITEM ), End,
							End,
							Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_WINDOWS_TITLE ),
								Child, objs[ MID_DCCINCOMING        ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_DCCINCOMING_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_DCCINCOMING_KEY ), End,
								Child, objs[ MID_DCCOUTGOIING       ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_DCCOUTGOING_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_DCCOUTGOING_KEY ), End,
								Child, objs[ MID_URLGRABBER         ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_URLGRABBER_ITEM  ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_URLGRABBER_KEY ), End,
								Child, objs[ MID_IGNORELIST         ] = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_IGNORELIST_ITEM  ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_IGNORELIST_KEY ), End,
							End,
						End,
			WindowContents, VGroup,
						Child, HGroup,
							Child, objs[ GID_CLOSETAB    ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_CLOSETAB_GAD ),
							Child, objs[ GID_TOPIC       ] = MUICreateString( MSG_MUICLASS_WINDOWCHAT_TOPIC_HELP-1, TOPIC_SIZEOF ),
							Child, BalanceObject, End,
							Child, objs[ GID_MODET       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODET_GAD ),
							Child, objs[ GID_MODEN       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEN_GAD ),
							Child, objs[ GID_MODES       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODES_GAD ),
							Child, objs[ GID_MODEI       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEI_GAD ),
							Child, objs[ GID_MODEP       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEP_GAD ),
							Child, objs[ GID_MODEM       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEM_GAD ),
							Child, objs[ GID_MODEB       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEB_GAD ),
							Child, objs[ GID_MODEK       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEK_GAD ),
							Child, HGroup, MUIA_HorizWeight, 40,
								Child, objs[ GID_MODEKEYWORD ] = MUICreateString( MSG_MUICLASS_WINDOWCHAT_MODEKEYWORD_HELP-1, KEYWORDLIMIT_SIZEOF ),
								Child, objs[ GID_MODEL       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWCHAT_MODEL_GAD ),
								Child, objs[ GID_MODELIMIT   ] = MUICreateString( MSG_MUICLASS_WINDOWCHAT_MODELIMIT_HELP-1, USERLIMIT_SIZEOF ),
							End,
						End,
						Child, HGroup,
							Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHATLOG ] = ChatLogObject, End, End,
							Child, VGroup, MUIA_HorizWeight, 20,
								Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHATUSERLIST      ] = ChatUserListObject, End, MUIA_ShortHelp, LGS( MSG_MUICLASS_WINDOWCHAT_NICKLIST_HELP ), End,
								Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHATCHANNELLIST   ] = ChatChannelListObject, End, End,
							End,
						 End,
						Child, objs[ GID_CHATMESSAGE  ] = MUICreateString( MSG_MUICLASS_WINDOWCHAT_MESSAGE_HELP-1, MESSAGE_SIZEOF ),

					 End,
		TAG_DONE ) ) ) {
		ULONG i;
		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		SetAttrs( obj, TAG_MORE, msg->ops_AttrList );

		sprintf( &mccdata->mcc_WindowTitle[0], (const char *) LGS( MSG_MUICLASS_WINDOWCHAT_TITLE ), VERSION, REVISION );
		SetAttrs( obj, MUIA_Window_Title, mccdata->mcc_WindowTitle, TAG_DONE );

		for( i = FIRSTMENU_ITEM ; i <= LASTMENU_ITEM ; i++ ) {
			DoMethod( mccdata->mcc_ClassObjects[ i ], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, obj, 2, MM_WINDOWCHAT_MENUSELECT, i );
		}

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
//struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static ULONG OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWCHAT ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
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
			case MA_WINDOWCHAT_OBJECTNETWORK:
				mccdata->mcc_ClassObjects[ GID_NETWORK ] = (APTR) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
Object *winobj;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	mccdata->mcc_ClassObjects[ WID_SETTINGS ] = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );

	winobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWQUIT );
	DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, winobj, 3, MUIM_Set, MUIA_Window_Open, TRUE );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime, obj, 1, MM_WINDOWCHAT_CHANNELCHANGE );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATMESSAGE ], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, obj, 1, MM_WINDOWCHAT_MESSAGEENTERED );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_MenuSelect()
**
*/

/*************************************************************************/

static ULONG MM_MenuSelect( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_MENUSELECT *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );
Object *tmpobj;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	switch( msg->MenuID ) {

/* project menu */
		case MID_ABOUT:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWABOUT );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
		case MID_QUIT:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWQUIT );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
/* edit menu */

/* settings menu */
		case MID_SETTINGS:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWSETTINGS );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;

/* window menu */
		case MID_SETTINGSMUI:
			break;
		case MID_IGNORELIST:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWIGNORELIST );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
		case MID_URLGRABBER:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_OBJECTWINDOWURLGRABBER );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ColorChange()
**
*/

/*************************************************************************/

static ULONG MM_ColorChange( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG         ], MM_CHATLOG_PENSUPDATE         );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST    ], MM_CHATUSERLIST_PENSUPDATE    );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MM_CHATCHANNELLIST_PENSUPDATE );

	return( 0 );
}
/* \\\ */

/* /// MM_ChannelAdd()
**
*/

/*************************************************************************/

static ULONG MM_ChannelAdd( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELADD *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel *cc;
ULONG i;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	/* only add, if not already in list */
	for( i = 0 ;  ; i++ ) {
		cc = NULL;
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, i, &cc );
		if( cc ) {
			if( cc->cc_Channel == msg->Channel ) {
				return( 0 );
			}
		} else {
			break;
		}
	}

//	  DoMethod( mccdata->mcc_ClassObjects[ GID_CONNECTEDBUTTONS ], MM_CONNECTEDBUTTONS_ADD, msg->Channel );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_InsertSingle, msg->Channel, MUIV_NList_Insert_Bottom );

	return( 0 );
}
/* \\\ */
/* /// MM_ChannelRemove()
**
*/

/*************************************************************************/

static ULONG MM_ChannelRemove( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELREMOVE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel *cc;
ULONG i;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

//	  DoMethod( mccdata->mcc_ClassObjects[ GID_CONNECTEDBUTTONS ], MM_CONNECTEDBUTTONS_REMOVE, msg->Channel );

	for( i = 0 ;  ; i++ ) {
		cc = NULL;
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, i, &cc );
		if( cc ) {
			if( cc->cc_Channel == msg->Channel ) {
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_Remove, i );
				if( i ) {
					i--; /* next time this pos is a differen list entry different */
				}
			}
		} else {
			break;
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelChange()
**
*/

/*************************************************************************/

static ULONG MM_ChannelChange( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELCHANGE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel     *cc;
struct Channel         *c;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG      ], MUIM_NList_Clear );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Clear );

	cc = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cc );
	if( cc ) {
		if( ( c = cc->cc_Channel ) ) {
			struct Node *node;
			DoMethod( obj, MM_WINDOWCHAT_CHANNELCHANGETOPIC, c );
			for( node = (APTR) c->c_ChatLogList.lh_Head ; node->ln_Succ ; node = node->ln_Succ ) {
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MUIM_NList_InsertSingleWrap, node, MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT );
			}
			for( node = (APTR) c->c_ChatNickList.lh_Head ; node->ln_Succ ; node = node->ln_Succ ) {
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_InsertSingle, node, MUIV_NList_Insert_Bottom );
			}
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Sort );

		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelChangeTopic()
**
*/

/*************************************************************************/

static ULONG MM_ChannelChangeTopic( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELCHANGETOPIC *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel     *cc;
struct Channel         *c;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	cc = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cc );
	if( cc ) {
		if( ( c = cc->cc_Channel ) ) {
			char *topic;
			if( ( topic = c->c_Topic ) ) {
				SetAttrs( mccdata->mcc_ClassObjects[ GID_TOPIC ], MUIA_NoNotify, TRUE, MUIA_String_Contents, topic, TAG_DONE );
			}
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ChannelNickAdd()
**
*/

/*************************************************************************/

static ULONG MM_ChannelNickAdd( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELNICKADD *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel *cc;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	cc = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cc );
	if( cc ) {
		/* is this channel visible right now? */
		if( cc->cc_Channel == msg->Channel ) { /* yes, then update user list */
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_InsertSingle, msg->ChatNickEntry, MUIV_NList_Insert_Bottom );
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Sort );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ChannelNickRemove()
**
*/

/*************************************************************************/

static ULONG MM_ChannelNickRemove( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_CHANNELNICKREMOVE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatNick *cn;
ULONG i;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ;  ; i++ ) {
		cn = NULL;
		DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_GetEntry, i, &cn );
		if( cn ) {
			if( cn->cn_ChatNickEntry == msg->ChatNickEntry ) {
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ], MUIM_NList_Remove, i );
				break;
			}
		} else {
			break;
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_MessageReceived()
**
*/

/*************************************************************************/

static ULONG MM_MessageReceived( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_MESSAGERECEIVED *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct Channel *c;
struct ChatChannel *cc;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	/* pointer magic */

	c = (APTR) ( ( (IPTR) List_GetListFromNode( msg->ChatLogEntry ) ) - (IPTR) offsetof( struct Channel, c_ChatLogList ) );

/* is this channel visible right now? */

	cc = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cc );
	if( cc ) {
		if( cc->cc_Channel == c ) {
		/* yes, so add to log */
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MUIM_NList_InsertSingleWrap, msg->ChatLogEntry, MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_MessageEntered()
**
*/

/*************************************************************************/

static ULONG MM_MessageEntered( struct IClass *cl, Object *obj, struct MP_WINDOWCHAT_MESSAGEENTERED *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ChatChannel *cc;
struct Server *s;

	debug( "%s (%ld) %s - Class: 0x00025165x Object: 0x00025165x \n", __FILE__, __LINE__, __func__, cl, obj );

	cc = NULL;
	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATCHANNELLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &cc );
	if( cc ) {
		/* pointer magic */
		s = (APTR) ( ( (IPTR) List_GetListFromNode( cc->cc_Channel ) ) - (IPTR) offsetof( struct Server, s_ChannelList ) );
		strcpy( mccdata->mcc_CommandBuffer, (char*) "PRIVMSG " );
		strcat( mccdata->mcc_CommandBuffer, cc->cc_Channel->c_Name );
		strcat( mccdata->mcc_CommandBuffer, (char*) " :" );
		strcat( mccdata->mcc_CommandBuffer, (char *) MUIGetVar( mccdata->mcc_ClassObjects[ GID_CHATMESSAGE ], MUIA_String_Contents ) );
		DoMethod( mccdata->mcc_ClassObjects[ GID_NETWORK ], MM_NETWORK_SERVERMESSAGESEND, s, mccdata->mcc_CommandBuffer );
		/* geit FIXME: This should be done in subclass of string field, which also keeps command history */
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHATMESSAGE ], MUIA_String_Contents, "", TAG_DONE );
		SetAttrs( obj, MUIA_Window_ActiveObject, mccdata->mcc_ClassObjects[ GID_CHATMESSAGE ], TAG_DONE );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowChat_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowChat_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                 ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                         : return( OM_Dispose             ( cl, obj, (APTR) msg ) );
																		   	
		case OM_GET                             : return( OM_Get                 ( cl, obj, (APTR) msg ) );
		case OM_SET                             : return( OM_Set                 ( cl, obj, (APTR) msg ) );

		case MUIM_Window_Setup                  : return( OM_Setup               ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCHAT_MENUSELECT           : return( MM_MenuSelect          ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_COLORCHANGE          : return( MM_ColorChange         ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCHAT_MESSAGEENTERED       : return( MM_MessageEntered      ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_MESSAGERECEIVED      : return( MM_MessageReceived     ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELADD           : return( MM_ChannelAdd          ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELREMOVE        : return( MM_ChannelRemove       ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELCHANGE        : return( MM_ChannelChange       ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELCHANGETOPIC   : return( MM_ChannelChangeTopic  ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCHAT_CHANNELNICKADD       : return( MM_ChannelNickAdd      ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCHAT_CHANNELNICKREMOVE    : return( MM_ChannelNickRemove   ( cl, obj, (APTR) msg ) );

    }

	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowChat_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowChat_InitClass( void )
{
	appclasses[ CLASSID_WINDOWCHAT ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowChat_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWCHAT ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowChat_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowChat_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWCHAT ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWCHAT ] );
		appclasses[ CLASSID_WINDOWCHAT ] = NULL;
    }
}
/* \\\ */




