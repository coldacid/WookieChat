/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingsgui.c
*/

#define MUI_OBSOLETE
#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>
#include <string.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsgui.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_TABBUTTON = 0,
GID_TABSERVER,
GID_NICKLISTWIDTH,
GID_NICKLISTHEIGHT,
GID_NICKLISTGFXINFO,
GID_NICKLISTREMCHARS,
GID_SMILIES,
GID_SMILIESTHEME,
GID_TIMESHOW,
GID_TIMEFORMAT,
GID_CHANNELVIEWTIMEWIDTH,
GID_CHANNELVIEWNICKWIDTH,
GID_CHANNELVIEWSPACING,
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
	{ GID_TABBUTTON           , OID_GUI_TABBUTTON           , MUIA_Selected       , (LONG) 0 },
	{ GID_TABSERVER           , OID_GUI_TABSERVER           , MUIA_Selected       , (LONG) 0 },
	{ GID_NICKLISTWIDTH       , OID_GUI_NICKLISTWIDTH       , MUIA_Slider_Level   , (LONG) 70 },
	{ GID_NICKLISTHEIGHT      , OID_GUI_NICKLISTHEIGHT      , MUIA_Slider_Level   , (LONG) 200 },
	{ GID_NICKLISTGFXINFO     , OID_GUI_NICKLISTGFXINFO     , MUIA_Selected       , (LONG) 1 },
	{ GID_NICKLISTREMCHARS    , OID_GUI_NICKLISTREMCHARS    , MUIA_Selected       , (LONG) 1 },
	{ GID_SMILIES             , OID_GUI_SMILIES             , MUIA_Selected       , (LONG) 1 },
	{ GID_SMILIESTHEME        , OID_GUI_SMILIESTHEME        , MUIA_Cycle_Active   , (LONG) 0 },
	{ GID_TIMESHOW            , OID_GUI_TIMESHOW            , MUIA_Selected       , (LONG) 1 },
	{ GID_TIMEFORMAT          , OID_GUI_TIMEFORMAT          , MUIA_String_Contents, (LONG) "[%h:%m:%s]" },
	{ GID_CHANNELVIEWTIMEWIDTH, OID_GUI_CHANNELVIEWTIMEWIDTH, MUIA_Slider_Level   , (LONG) 120 },
	{ GID_CHANNELVIEWNICKWIDTH, OID_GUI_CHANNELVIEWNICKWIDTH, MUIA_Slider_Level   , (LONG) 250 },
	{ GID_CHANNELVIEWSPACING  , OID_GUI_CHANNELVIEWSPACING  , MUIA_Slider_Level   , (LONG) 8 },
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
#define TIMEFORMAT_SIZEOF 0x10
#define NICKLISTWIDTH_MAX 0x2000
#define NICKLISTHEIGHT_MAX 0x2000
#define CHANNELVIEWTIMEWIDTH_MAX  300
#define CHANNELVIEWNICKWIDTH_MAX  300
#define CHANNELVIEWSPACING_MAX    100

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];
static STRPTR TAB_CYCLE_SMILIES[ MSG_CY_BLACK - MSG_CY_TRANSPARENT + 2 ];

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, TRUE,
					Child, HVSpace,
					Child, VGroup,
						Child, HVSpace,

						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_TABS_INFO ),
						Child, HGroup,
							Child, objs[ GID_TABBUTTON          ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGUI_TABBUTTON_GAD, FALSE ),
							Child, HGroup,
								Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_TABBUTTON_GAD ),
								Child, HVSpace,
							End,
						End,
						Child, HGroup,
							Child, objs[ GID_TABSERVER          ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGUI_TABSERVER_GAD, FALSE ),
							Child, HGroup,
								Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_TABSERVER_GAD ),
								Child, HVSpace,
							End,
						End,

						Child, VSpace(5),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_NICKLIST_INFO ),
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGUI_NICKLISTWIDTH_GAD ),
							Child, objs[ GID_NICKLISTWIDTH  ] = MUICreateSlider( MSG_MUICLASS_SETTINGSGUI_NICKLISTWIDTH_GAD, 0, 0, NICKLISTWIDTH_MAX, MSG_SL_PIXEL, 20 ),
							Child, HVSpace,
						End,
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGUI_NICKLISTHEIGHT_GAD ),
							Child, objs[ GID_NICKLISTHEIGHT ] = MUICreateSlider( MSG_MUICLASS_SETTINGSGUI_NICKLISTHEIGHT_GAD, 0, 0, NICKLISTHEIGHT_MAX, MSG_SL_PIXEL, 20 ),
							Child, HVSpace,
						End,
						Child, HGroup,
							Child, objs[ GID_NICKLISTGFXINFO ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGUI_NICKLISTGFXINFO_GAD, FALSE ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_NICKLISTGFXINFO_GAD ),
							Child, HVSpace,
						End,
						Child, HGroup,
							Child, objs[ GID_NICKLISTREMCHARS ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGUI_NICKLISTREMOVECHARS_GAD, FALSE ),
							Child, HGroup,
								Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_NICKLISTREMOVECHARS_GAD ),
								Child, HVSpace,
							End,
						End,

						Child, VSpace(5),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_CHANNELVIEW_INFO ),
						Child, HGroup,
							Child, objs[ GID_SMILIES ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGUI_SMILIES_GAD, FALSE ),
							Child, HGroup,
								Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_SMILIES_GAD ),
								Child, objs[ GID_SMILIESTHEME    ] = MUICreateCycle( MSG_MUICLASS_SETTINGSGUI_SMILIESTHEME_GAD, &TAB_CYCLE_SMILIES, MSG_CY_TRANSPARENT, MSG_CY_BLACK ),
								Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_SMILIESTHEMEBACKGROUND_INFO ),
								Child, HVSpace,
							End,
						End,
						Child, HGroup,
							Child, objs[ GID_TIMESHOW               ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSGUI_TIMESHOW_GAD, TRUE ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_TIMESHOW_GAD ),
							Child, objs[ GID_TIMEFORMAT             ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSGUI_TIMEFORMAT_GAD, TIMEFORMAT_SIZEOF ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSGUI_CHANNELVIEWTIMEWIDTH_GAD ),
							Child, objs[ GID_CHANNELVIEWTIMEWIDTH ] = MUICreateSlider( MSG_MUICLASS_SETTINGSGUI_CHANNELVIEWTIMEWIDTH_GAD, 0, 0, CHANNELVIEWTIMEWIDTH_MAX, MSG_SL_PIXEL, 15 ),
							Child, HVSpace,
						End,
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGUI_CHANNELVIEWNICKWIDTH_GAD ),
							Child, objs[ GID_CHANNELVIEWNICKWIDTH ] = MUICreateSlider( MSG_MUICLASS_SETTINGSGUI_CHANNELVIEWNICKWIDTH_GAD, 0, 0, CHANNELVIEWNICKWIDTH_MAX, MSG_SL_PIXEL, 30 ),
							Child, HVSpace,
						End,
						Child, HGroup,
							Child, MUICreateLabel( MSG_MUICLASS_SETTINGSGUI_CHANNELVIEWSPACING_GAD ),
							Child, objs[ GID_CHANNELVIEWSPACING   ] = MUICreateSlider( MSG_MUICLASS_SETTINGSGUI_CHANNELVIEWSPACING_GAD, 0, 0, CHANNELVIEWSPACING_MAX, MSG_SL_PIXEL, 30 ),
							Child, HVSpace,
						End,
						Child, HVSpace,
					End,
					Child, HVSpace,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_SMILIES  ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_SMILIESTHEME         ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );
		DoMethod( objs[ GID_TIMESHOW ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_TIMEFORMAT           ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );
		DoMethod( objs[ GID_TIMESHOW ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_CHANNELVIEWTIMEWIDTH ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );

		DoMethod( obj, MM_SETTINGSGUI_RESETTODEFAULTS );

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

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSGUI_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].ObjectID == msg->ObjectID ) {
			return( (ULONG) MUIGetVar( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr ) );
		}
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsGUI_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsGUI_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                              : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSGUI_RESETTODEFAULTS     : return( MM_ResetToDefaults         ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSGUI_READCONFIG          : return( MM_ReadConfig              ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsGUI_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsGUI_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSGUI ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsGUI_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSGUI ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsGUI_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsGUI_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSGUI ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSGUI ] );
		appclasses[ CLASSID_SETTINGSGUI ] = NULL;
    }
}
/* \\\ */



