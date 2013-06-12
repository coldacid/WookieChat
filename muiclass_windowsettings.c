/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowsettings.c
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
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsnick.h"
#include "muiclass_settingsgeneral.h"
#include "muiclass_settingscolor.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

#define DEFAULT_SETTINGSPATH            "PROGDIR:Prefs"
#define DEFAULT_SETTINGSPRESETSPATH     DEFAULT_SETTINGSPATH "/Presets"

#define DEFAULT_SETTINGSNICK_NAME       DEFAULT_SETTINGSPATH "/nick.prefs"
#define DEFAULT_SETTINGSCOLOR_NAME      DEFAULT_SETTINGSPATH "/color.prefs"

/*
** gadgets used by this class
*/

enum
{
GID_PAGELIST = 0,
GID_PAGEGROUP,
GID_NICK,
GID_GENERAL,
GID_COLOR,
GID_SAVE,
GID_USE,
GID_CANCEL,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/* /// GlobalReadConfig()
**
*/

/*************************************************************************/

ULONG GlobalReadConfig( ULONG objectid )
{
Object *settobj;
ULONG result = 0;

	if( ( settobj = (Object *) MUIGetVar( application, MA_APPLICATION_WINDOWSETTINGS ) ) ) {
		result = DoMethod( settobj, MM_WINDOWSETTINGS_READCONFIG, objectid );
	}
	return( (ULONG) result );
}
/* \\\ */

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];
ULONG i;

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWSETTINGS_TITLE ),
			MUIA_Window_ID               , MAKE_ID('S','E','T','T'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
				Child, HGroup,
					Child, objs[ GID_PAGELIST ] = NListviewObject, MUIA_NListview_NList, NListObject,
															MUIA_Frame               , MUIV_Frame_InputList,
															MUIA_NList_ConstructHook , MUIV_NList_ConstructHook_String,
															MUIA_NList_DestructHook  , MUIV_NList_DestructHook_String,
													        MUIA_NList_AutoCopyToClip, TRUE,
															MUIA_NList_Input         , TRUE,
															End,
															MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Auto,
															MUIA_FixWidthTxt, "WWWWWWWWWWWWW",
														End,
					Child, HGroup,
						Child, HVSpace,
						Child, VGroup,
							Child, HVSpace,
							Child, objs[ GID_PAGEGROUP   ] = HGroup, MUIA_Group_PageMode, TRUE,
								Child, objs[ GID_NICK    ] = SettingsNickObject, End,
								Child, HVSpace,
								Child, objs[ GID_GENERAL ] = SettingsGeneralObject, End,
								Child, objs[ GID_COLOR   ] = SettingsColorObject, End,
								Child, HVSpace,
								Child, HVSpace,
								Child, HVSpace,
								Child, HVSpace,
								Child, HVSpace,
								Child, HVSpace,
								Child, HVSpace,
							End,
							Child, HVSpace,
						End,
						Child, HVSpace,
					End,
				End,
				Child, HGroup,
					Child, objs[ GID_SAVE   ] = MUICreateButton( MSG_MUICLASS_SETTINGS_SAVE_GAD ),
					Child, objs[ GID_USE    ] = MUICreateButton( MSG_MUICLASS_SETTINGS_USE_GAD ),
					Child, objs[ GID_CANCEL ] = MUICreateButton( MSG_MUICLASS_SETTINGS_CANCEL_GAD ),
				End,
			End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		for( i = 0 ; i < SETTINGS_LAST ; i++ ) {
			DoMethod( objs[ GID_PAGELIST ], MUIM_NList_InsertSingle, LGS( MSG_PG_NICK + i ), MUIV_NList_Insert_Bottom );
		}


		/* create drawers */
		{ BPTR lock;
		if( ( lock = CreateDir( (_s_cs) DEFAULT_SETTINGSPATH ) ) ) {
			UnLock( lock );
		}
		if( ( lock = CreateDir( (_s_cs) DEFAULT_SETTINGSPRESETSPATH ) ) ) {
			UnLock( lock );
		}}

		SetAttrs( objs[ GID_PAGELIST ], MUIA_NList_Active, SETTINGS_NICK, TAG_DONE );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	DoMethod( obj                                      , MUIM_Notify, MUIA_Window_CloseRequest, TRUE          , obj                  , 3, MUIM_Set, MUIA_Window_Open     , FALSE );
	DoMethod( obj                                      , MUIM_Notify, MUIA_Window_CloseRequest, TRUE          , obj                  , 2, MUIM_Application_Load, MUIV_Application_Load_ENV    );
	DoMethod( mccdata->mcc_ClassObjects[ GID_PAGELIST ], MUIM_Notify, MUIA_NList_Active       , MUIV_EveryTime, mccdata->mcc_ClassObjects[ GID_PAGEGROUP ], 3, MUIM_Set, MUIA_Group_ActivePage, MUIV_TriggerValue );

	DoMethod( mccdata->mcc_ClassObjects[ GID_SAVE     ], MUIM_Notify, MUIA_Pressed            , FALSE         , _app(obj)            , 3, MUIM_Set, MUIA_Window_Open     , FALSE );
	DoMethod( mccdata->mcc_ClassObjects[ GID_SAVE     ], MUIM_Notify, MUIA_Pressed            , FALSE         , _app(obj)            , 2, MUIM_Application_Save, MUIV_Application_Save_ENVARC );
	DoMethod( mccdata->mcc_ClassObjects[ GID_USE      ], MUIM_Notify, MUIA_Pressed            , FALSE         , _app(obj)            , 3, MUIM_Set, MUIA_Window_Open     , FALSE );
	DoMethod( mccdata->mcc_ClassObjects[ GID_USE      ], MUIM_Notify, MUIA_Pressed            , FALSE         , _app(obj)            , 2, MUIM_Application_Save, MUIV_Application_Save_ENV    );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CANCEL   ], MUIM_Notify, MUIA_Pressed            , FALSE         , _app(obj)            , 3, MUIM_Set, MUIA_Window_Open     , FALSE );
	DoMethod( mccdata->mcc_ClassObjects[ GID_CANCEL   ], MUIM_Notify, MUIA_Pressed            , FALSE         , _app(obj)            , 2, MUIM_Application_Load, MUIV_Application_Load_ENV    );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_WINDOWSETTINGS_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG result;

	if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_NICK ], MM_SETTINGSNICK_READCONFIG, msg->ObjectID ) ) ) {
		if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_GENERAL ], MM_SETTINGSGENERAL_READCONFIG, msg->ObjectID ) ) ) {
			if( !( result = DoMethod( mccdata->mcc_ClassObjects[ GID_COLOR ], MM_SETTINGSCOLOR_READCONFIG, msg->ObjectID ) ) ) {
			}
		}
	}
	return( (ULONG) result );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowSettings_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowSettings_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New         ( cl, obj, (APTR) msg ) );
		case MUIM_Window_Setup                  : return( OM_Setup       ( cl, obj, (APTR) msg ) );
		case MM_WINDOWSETTINGS_READCONFIG       : return( MM_ReadConfig  ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowSettings_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowSettings_InitClass( void )
{
	appclasses[ CLASSID_WINDOWSETTINGS ] = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowSettings_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWSETTINGS ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowSettings_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowSettings_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWSETTINGS ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWSETTINGS ] );
		appclasses[ CLASSID_WINDOWSETTINGS ] = NULL;
    }
}
/* \\\ */



