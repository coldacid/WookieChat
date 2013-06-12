/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingscolor.c
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
#include "muiclass_windowsettings.h"
#include "muiclass_settingscolor.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

#define LINEBUFFER_SIZEOF 0x2000


/*
** gadgets used by this class
*/

enum
{
GID_BACKGROUND = 0,
GID_NICKLISTBG,
GID_TABLISTBG,
GID_NORMAL,
GID_NICKLISTTX,
GID_TABSPEN,
GID_JOIN,
GID_PART,
GID_QUIT,
GID_MODES,
GID_CTCP,
GID_ACTIONS,
GID_INFO,
GID_OWNTEXT,
GID_HIGHLIGHT,
GID_NOTICES,
GID_INVITE,
GID_KICK,
GID_NICKCHANGE,
GID_TOPIC,
GID_WALLOPS,
GID_ACTIVITY,
GID_CHATACTIVITY,
GID_HIGHLIGHTACTIVITY,
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
	{ GID_BACKGROUND       , OID_COL_BACKGROUND       , MUIA_Pendisplay_Spec, (LONG) "2:00000000,00000000,00000000" },
	{ GID_NICKLISTBG       , OID_COL_NICKLISTBG       , MUIA_Pendisplay_Spec, (LONG) "2:1D451D45,2B5D2B5D,8D798D79" },
	{ GID_TABLISTBG        , OID_COL_TABLISTBG        , MUIA_Pendisplay_Spec, (LONG) "r52215221,FFFFFFFF,2D9B2D9B" },
	{ GID_NORMAL           , OID_COL_NORMAL           , MUIA_Pendisplay_Spec, (LONG) "r209C209C,A1AFA1AF,1A271A27" },
	{ GID_NICKLISTTX       , OID_COL_NICKLISTTX       , MUIA_Pendisplay_Spec, (LONG) "rE7D8E7D8,25A425A4,FFFFFFFF" },
	{ GID_TABSPEN          , OID_COL_TABSPEN          , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,8FC08FC0,12D012D0" },
	{ GID_JOIN             , OID_COL_JOIN             , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,0D620D62,00000000" },
	{ GID_PART             , OID_COL_PART             , MUIA_Pendisplay_Spec, (LONG) "rFFFF0000,FFFF0000,FFFF0000" },
	{ GID_QUIT             , OID_COL_QUIT             , MUIA_Pendisplay_Spec, (LONG) "rFFFDFFFD,FFFBFFFB,FFFFFFFF" },
	{ GID_MODES            , OID_COL_MODES            , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,AEACAEAC,D159D159" },
	{ GID_CTCP             , OID_COL_CTCP             , MUIA_Pendisplay_Spec, (LONG) "r7C3B7C3B,FFFFFFFF,F8CEF8CE" },
	{ GID_ACTIONS          , OID_COL_ACTIONS          , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,FBCCFBCC,25A225A2" },
	{ GID_INFO             , OID_COL_INFO             , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,1A591A59,25292529" },
	{ GID_OWNTEXT          , OID_COL_OWNTEXT          , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,12D012D0,168E168E" },
	{ GID_HIGHLIGHT        , OID_COL_HIGHLIGHT        , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,24CE24CE,12D212D2" },
	{ GID_NOTICES          , OID_COL_NOTICES          , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,8F098F09,1E1D1E1D" },
	{ GID_INVITE           , OID_COL_INVITE           , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,96409640,34B234B2" },
	{ GID_KICK             , OID_COL_KICK             , MUIA_Pendisplay_Spec, (LONG) "r61E061E0,FFFFFFFF,8CDC8CDC" },
	{ GID_NICKCHANGE       , OID_COL_NICKCHANGE       , MUIA_Pendisplay_Spec, (LONG) "r43BE43BE,509F509F,FFFFFFFF" },
	{ GID_TOPIC            , OID_COL_TOPIC            , MUIA_Pendisplay_Spec, (LONG) "rFFFFFFFF,07830783,0B570B57" },
	{ GID_WALLOPS          , OID_COL_WALLOPS          , MUIA_Pendisplay_Spec, (LONG) "r527F527F,FFFFFFFF,2D292D29" },
	{ GID_ACTIVITY         , OID_COL_ACTIVITY         , MUIA_Pendisplay_Spec, (LONG) "rFFFDFFFD,FFFCFFFC,FFFFFFFF" },
	{ GID_CHATACTIVITY     , OID_COL_CHATACTIVITY     , MUIA_Pendisplay_Spec, (LONG) "2:DE50DE50,DE4ADE4A,DE47DE47" },
	{ GID_HIGHLIGHTACTIVITY, OID_COL_HIGHLIGHTACTIVITY, MUIA_Pendisplay_Spec, (LONG) "2:00000000,00000000,00000000" },
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

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, TRUE,
				Child, ColGroup(6),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_BACKGROUND_GAD ),
					Child, objs[ GID_BACKGROUND        ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_BACKGROUND_GAD       , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_NICKLISTBG_GAD ),
					Child, objs[ GID_NICKLISTBG        ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_NICKLISTBG_GAD       , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_TABLISTBG_GAD ),
					Child, objs[ GID_TABLISTBG         ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_TABLISTBG_GAD        , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_NORMAL_GAD ),
					Child, objs[ GID_NORMAL            ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_NORMAL_GAD           , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_NICKLISTTX_GAD ),
					Child, objs[ GID_NICKLISTTX        ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_NICKLISTTX_GAD       , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_TABSPEN_GAD ),
					Child, objs[ GID_TABSPEN           ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_TABSPEN_GAD          , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_JOIN_GAD ),
					Child, objs[ GID_JOIN              ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_JOIN_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_PART_GAD ),
					Child, objs[ GID_PART              ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_PART_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_QUIT_GAD ),
					Child, objs[ GID_QUIT              ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_QUIT_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_MODES_GAD ),
					Child, objs[ GID_MODES             ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_MODES_GAD            , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_CTCP_GAD ),
					Child, objs[ GID_CTCP              ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_CTCP_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_ACTIONS_GAD ),
					Child, objs[ GID_ACTIONS           ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_ACTIONS_GAD          , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_INFO_GAD ),
					Child, objs[ GID_INFO              ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_INFO_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_OWNTEXT_GAD ),
					Child, objs[ GID_OWNTEXT           ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_OWNTEXT_GAD          , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_HIGHLIGHT_GAD ),
					Child, objs[ GID_HIGHLIGHT         ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_HIGHLIGHT_GAD        , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_NOTICES_GAD ),
					Child, objs[ GID_NOTICES           ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_NOTICES_GAD          , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_INVITE_GAD ),
					Child, objs[ GID_INVITE            ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_INVITE_GAD           , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_KICK_GAD ),
					Child, objs[ GID_KICK              ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_KICK_GAD             , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_NICKCHANGE_GAD ),
					Child, objs[ GID_NICKCHANGE        ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_NICKCHANGE_GAD       , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_TOPIC_GAD ),
					Child, objs[ GID_TOPIC             ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_TOPIC_GAD            , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_WALLOPS_GAD ),
					Child, objs[ GID_WALLOPS           ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_WALLOPS_GAD          , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_ACTIVITY_GAD ),
					Child, objs[ GID_ACTIVITY          ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_ACTIVITY_GAD         , MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_CHATACTIVITY_GAD ),
					Child, objs[ GID_CHATACTIVITY      ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_CHATACTIVITY_GAD     ,  MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSCOLOR_HIGHLIGHTACTIVITY_GAD ),
					Child, objs[ GID_HIGHLIGHTACTIVITY ] = MUICreatePoppen( MSG_MUICLASS_SETTINGSCOLOR_HIGHLIGHTACTIVITY_GAD,  MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE ),
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( obj, MM_SETTINGSCOLOR_RESETTODEFAULTS );

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

/* Poppen are not supported by dataspace, so we need to adapt */

/* /// OM_Import()
**
*/

/*************************************************************************/

static ULONG OM_Import( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].Attr == MUIA_Pendisplay_Spec ) {
			MUIDataspaceExportPoppen( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], msg->dataspace, TAB_CONFIGITEMS[ i ].ObjectID );
		}
	}
	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Export()
**
*/

/*************************************************************************/

static ULONG OM_Export( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].Attr == MUIA_Pendisplay_Spec ) {
			MUIDataspaceExportPoppen( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], msg->dataspace, TAB_CONFIGITEMS[ i ].ObjectID );
		}
	}

	return( DoSuperMethodA( cl, obj, msg ) );
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

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSCOLOR_READCONFIG *msg )
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

/* /// MCC_SettingsColor_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsColor_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                            : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                        : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );
		case MUIM_Import                       : return( OM_Import                        ( cl, obj, (APTR) msg ) );
		case MUIM_Export                       : return( OM_Export                        ( cl, obj, (APTR) msg ) );

		case MM_SETTINGSCOLOR_RESETTODEFAULTS  : return( MM_ResetToDefaults               ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSCOLOR_READCONFIG       : return( MM_ReadConfig                    ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsColor_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsColor_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSCOLOR ] = MUI_CreateCustomClass( NULL, MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsColor_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSCOLOR ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsColor_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsColor_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSCOLOR ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSCOLOR ] );
		appclasses[ CLASSID_SETTINGSCOLOR ] = NULL;
    }
}
/* \\\ */



