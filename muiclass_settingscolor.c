/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowcolorsettings.c
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

#define GID_LASTCOLOR  GID_HIGHLIGHTACTIVITY
#define GID_FIRSTCOLOR GID_BACKGROUND
#define COLORS_NUMBEROF ( GID_LASTCOLOR - GID_FIRSTCOLOR + 1 )

/*
** color defaults
*/

static APTR TAB_COLOR_DEFAULTS[] = {
	(APTR) GID_BACKGROUND       , "2:00000000,00000000,00000000",
	(APTR) GID_NICKLISTBG       , "2:1D451D45,2B5D2B5D,8D798D79",
	(APTR) GID_TABLISTBG        , "r52215221,FFFFFFFF,2D9B2D9B",
	(APTR) GID_NORMAL           , "r209C209C,A1AFA1AF,1A271A27",
	(APTR) GID_NICKLISTTX       , "rE7D8E7D8,25A425A4,FFFFFFFF",
	(APTR) GID_TABSPEN          , "rFFFFFFFF,8FC08FC0,12D012D0",
	(APTR) GID_JOIN             , "rFFFFFFFF,0D620D62,00000000",
	(APTR) GID_PART             , "rFFFF0000,FFFF0000,FFFF0000",
	(APTR) GID_QUIT             , "rFFFDFFFD,FFFBFFFB,FFFFFFFF",
	(APTR) GID_MODES            , "rFFFFFFFF,AEACAEAC,D159D159",
	(APTR) GID_CTCP             , "r7C3B7C3B,FFFFFFFF,F8CEF8CE",
	(APTR) GID_ACTIONS          , "rFFFFFFFF,FBCCFBCC,25A225A2",
	(APTR) GID_INFO             , "rFFFFFFFF,1A591A59,25292529",
	(APTR) GID_OWNTEXT          , "rFFFFFFFF,12D012D0,168E168E",
	(APTR) GID_HIGHLIGHT        , "rFFFFFFFF,24CE24CE,12D212D2",
	(APTR) GID_NOTICES          , "rFFFFFFFF,8F098F09,1E1D1E1D",
	(APTR) GID_INVITE           , "rFFFFFFFF,96409640,34B234B2",
	(APTR) GID_KICK             , "r61E061E0,FFFFFFFF,8CDC8CDC",
	(APTR) GID_NICKCHANGE       , "r43BE43BE,509F509F,FFFFFFFF",
	(APTR) GID_TOPIC            , "rFFFFFFFF,07830783,0B570B57",
	(APTR) GID_WALLOPS          , "r527F527F,FFFFFFFF,2D292D29",
	(APTR) GID_ACTIVITY         , "rFFFDFFFD,FFFCFFFC,FFFFFFFF",
	(APTR) GID_CHATACTIVITY     , "2:DE50DE50,DE4ADE4A,DE47DE47",
	(APTR) GID_HIGHLIGHTACTIVITY, "2:00000000,00000000,00000000",
	NULL, NULL,
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	struct MUI_PenSpec    *mcc_PenDisplaySpecs[ COLORS_NUMBEROF ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];
ULONG i;

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

		for( i = 0 ; i < COLORS_NUMBEROF ; i++ ) {
			SetAttrs( objs[ GID_FIRSTCOLOR + i ], MUIA_ObjectID, OID_SETTINGSCOLOR + 1 + i, TAG_DONE );
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

/* Poppen are not supported by dataspace, so we need to adapt */

/* /// OM_Import()
**
*/

/*************************************************************************/

static ULONG OM_Import( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; i < COLORS_NUMBEROF ; i++ ) {
		MUIDataspaceExportPoppen( mccdata->mcc_ClassObjects[ GID_FIRSTCOLOR + i ], msg->dataspace, OID_SETTINGSCOLOR + 1 + i );
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

	for( i = 0 ; i < COLORS_NUMBEROF ; i++ ) {
		MUIDataspaceExportPoppen( mccdata->mcc_ClassObjects[ GID_FIRSTCOLOR + i ], msg->dataspace, OID_SETTINGSCOLOR + 1 + i );
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

	for( i = 0 ; TAB_COLOR_DEFAULTS[ i + 1 ] ; i+=2 ) {
		SetAttrs( mccdata->mcc_ClassObjects[ (ULONG) TAB_COLOR_DEFAULTS[ i ] ], MUIA_Pendisplay_Spec, (ULONG) TAB_COLOR_DEFAULTS[ i + 1 ], TAG_DONE );
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



