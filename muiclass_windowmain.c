/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowmain.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>
#include <string.h>
#include <stdio.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowmain.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowsettings.h"
#include "muiclass_nicklist.h"
#include "muiclass_channellist.h"
#include "muiclass_channel.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_MENUSTRIP = 0,
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
GID_NICKLIST,
GID_CHANNELLIST,
GID_CHANNEL,
GID_MESSAGE,
GID_LAST
};

#define WINDOWTITLE_SIZEOF 0x100
#define TOPIC_SIZEOF 0x400
#define KEYWORDLIMIT_SIZEOF 40
#define USERLIMIT_SIZEOF    40
#define MESSAGE_SIZEOF      40

#define FIRSTMENU_ITEM MID_SELECTSERVER	
#define LASTMENU_ITEM MID_IGNORELIST
 
/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	char                   mcc_WindowTitle[ WINDOWTITLE_SIZEOF + 2 ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWMAIN_TITLE ),
			MUIA_Window_ID               , MAKE_ID('M','A','I','N'),
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
							Child, objs[ GID_CLOSETAB    ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_CLOSETAB_GAD ),
							Child, objs[ GID_TOPIC       ] = MUICreateString( MSG_MUICLASS_WINDOWMAIN_TOPIC_HELP-1, TOPIC_SIZEOF ),
							Child, BalanceObject, End,
							Child, objs[ GID_MODET       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODET_GAD ),
							Child, objs[ GID_MODEN       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODEN_GAD ),
							Child, objs[ GID_MODES       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODES_GAD ),
							Child, objs[ GID_MODEI       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODEI_GAD ),
							Child, objs[ GID_MODEP       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODEP_GAD ),
							Child, objs[ GID_MODEM       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODEM_GAD ),
							Child, objs[ GID_MODEB       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODEB_GAD ),
							Child, objs[ GID_MODEK       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODEK_GAD ),
							Child, objs[ GID_MODEKEYWORD ] = MUICreateString( MSG_MUICLASS_WINDOWMAIN_MODEKEYWORD_HELP-1, KEYWORDLIMIT_SIZEOF ),
							Child, objs[ GID_MODEL       ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWMAIN_MODEL_GAD ),
							Child, objs[ GID_MODELIMIT   ] = MUICreateString( MSG_MUICLASS_WINDOWMAIN_MODELIMIT_HELP-1, USERLIMIT_SIZEOF ),
						End,
						Child, HGroup,
							Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHANNEL ] = ChannelObject, End, End,
							Child, VGroup,
								Child, NListviewObject, MUIA_NListview_NList, objs[ GID_NICKLIST    ] = NickListObject, End, MUIA_ShortHelp, LGS( MSG_MUICLASS_WINDOWMAIN_NICKLIST_HELP ), End,
								Child, NListviewObject, MUIA_NListview_NList, objs[ GID_CHANNELLIST ] = ChannelListObject, End, End,
							End,
						 End,
						Child, objs[ GID_MESSAGE         ] = MUICreateString( MSG_MUICLASS_WINDOWMAIN_MESSAGE_HELP-1, MESSAGE_SIZEOF ),

					 End,
		TAG_DONE ) ) ) {
		ULONG i;
		struct mccdata *mccdata = INST_DATA( cl, obj );


		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		sprintf( &mccdata->mcc_WindowTitle[0], (const char *) LGS( MSG_MUICLASS_WINDOWMAIN_TITLE ), VERSION, REVISION );
		SetAttrs( obj, MUIA_Window_Title, mccdata->mcc_WindowTitle, TAG_DONE );

		for( i = FIRSTMENU_ITEM ; i <= LASTMENU_ITEM ; i++ ) {
			DoMethod( mccdata->mcc_ClassObjects[ i ], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, obj, 2, MM_WINDOWMAIN_MENUSELECT, i );
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

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */

/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
{
Object *winobj;

	winobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWQUIT );
	DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, winobj, 3, MUIM_Set, MUIA_Window_Open, TRUE );

	winobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWSETTINGS );


	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_MenuSelect()
**
*/

/*************************************************************************/

static ULONG MM_MenuSelect( struct IClass *cl, Object *obj, struct MP_WINDOWMAIN_MENUSELECT *msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );
Object *tmpobj;

	switch( msg->MenuID ) {

/* project menu */
		case MID_ABOUT:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWABOUT );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
		case MID_QUIT:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWQUIT );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
/* edit menu */

/* settings menu */
		case MID_SETTINGS:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWSETTINGS );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;

/* window menu */
		case MID_SETTINGSMUI:
			break;
		case MID_IGNORELIST:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWIGNORELIST );
			SetAttrs( tmpobj, MUIA_Window_Open, TRUE, TAG_DONE );
			break;
		case MID_URLGRABBER:
			tmpobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWURLGRABBER );
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
Object *settingsobj;

	if( ( settingsobj = (Object *) MUIGetVar( _app(obj), MA_APPLICATION_WINDOWSETTINGS ) ) ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNEL     ], MUIA_Background, DoMethod( settingsobj, MM_WINDOWSETTINGS_READCONFIG, OID_COL_CHANNELBG ), TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_NICKLIST    ], MUIA_Background, DoMethod( settingsobj, MM_WINDOWSETTINGS_READCONFIG, OID_COL_NICKLISTBG ), TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIA_Background, DoMethod( settingsobj, MM_WINDOWSETTINGS_READCONFIG, OID_COL_TABLISTBG ), TAG_DONE );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowMain_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowMain_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );

		case MUIM_Window_Setup               : return( OM_Setup                         ( cl, obj, (APTR) msg ) );

		case MM_WINDOWMAIN_MENUSELECT        : return( MM_MenuSelect                    ( cl, obj, (APTR) msg ) );
		case MM_WINDOWMAIN_COLORCHANGE       : return( MM_ColorChange                   ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowMain_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowMain_InitClass( void )
{
	appclasses[ CLASSID_WINDOWMAIN ] = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowMain_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWMAIN ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowMain_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowMain_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWMAIN ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWMAIN ] );
		appclasses[ CLASSID_WINDOWMAIN ] = NULL;
    }
}
/* \\\ */




