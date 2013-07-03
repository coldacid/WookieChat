/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingssound.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>
#include <string.h>

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingssound.h"
#include "version.h"

/*************************************************************************/

#define SAMPLENAME_SIZEOF 0x200
 
/*
** gadgets used by this class
*/

enum
{
GID_ENABLED = 0,
GID_MODEONTABOPENING,
GID_SPLONTABOPENING,
GID_MODEONHIGHLIGHT,
GID_SPLONHIGHLIGHT,
GID_MODEONPRIVMSG,
GID_SPLONPRIVMSG,
GID_CTCPSAMPLES,
GID_USEEXTERNALPLAYER,
GID_EXTERNALPLAYER,
GID_LAST,
/* these need no storage, so defined after GID_LAST */
GID_CMENU_RESETTODEFAULTS,
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
	{ GID_ENABLED           , OID_SND_ENABLE           , MUIA_Selected       , (LONG) 1 },
	{ GID_MODEONTABOPENING  , OID_SND_MODEONTABOPENING , MUIA_Cycle_Active   , (LONG) 1 },
	{ GID_SPLONTABOPENING   , OID_SND_SPLONTABOPENING  , MUIA_String_Contents, (LONG) DEFAULT_PRESETSSOUNDSPATH "/Eagh" },
	{ GID_MODEONHIGHLIGHT   , OID_SND_MODEONHIGHLIGHT  , MUIA_Cycle_Active   , (LONG) 1 },
	{ GID_SPLONHIGHLIGHT    , OID_SND_SPLONHIGHLIGHT   , MUIA_String_Contents, (LONG) DEFAULT_PRESETSSOUNDSPATH "/OhNo" },
	{ GID_MODEONPRIVMSG     , OID_SND_MODEONPRIVMSG    , MUIA_Cycle_Active   , (LONG) 1 },
	{ GID_SPLONPRIVMSG      , OID_SND_SPLONPRIVMSG     , MUIA_String_Contents, (LONG) DEFAULT_PRESETSSOUNDSPATH "/Beep" },
	{ GID_CTCPSAMPLES       , OID_SND_CTCPSAMPLES      , MUIA_String_Contents, (LONG) DEFAULT_PRESETSSOUNDSPATH "/" },
	{ GID_USEEXTERNALPLAYER , OID_SND_USEEXTERNALPLAYER, MUIA_Selected       , (LONG) 0 },
	{ GID_EXTERNALPLAYER    , OID_SND_EXTERNALPLAYER   , MUIA_String_Contents, (LONG) "/" },
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
#define SAMPLENAME_SIZEOF 0x200

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];
static STRPTR TAB_CYCLE_TABOPENMODES[ MSG_CY_WINDOWISINACTIVE - MSG_CY_NEVER + 2 ];
static STRPTR TAB_CYCLE_HIGHLIGHTMODES[ MSG_CY_TABISINACTIVE - MSG_CY_NEVER + 2 ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = (Object *) DoSuperNew( cl, obj,
					MUIA_Group_Horiz, TRUE,
					MUIA_ContextMenu,
							MenustripObject,
								Child, MenuObject, MUIA_Menu_Title, LGS( MSG_MUICLASS_SETTINGSSOUND_SOUND_CMENU ),
								Child, MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MUICLASS_SETTINGSSOUND_RESETTODEFAULTS_CMENU ), MUIA_UserData, GID_CMENU_RESETTODEFAULTS, End,
							End,
						End,
					//Child, HVSpace,
					Child, VGroup,
						Child, HVSpace,
						Child, HGroup,
							Child, objs[ GID_ENABLED ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSSOUND_ENABLED_GAD, TRUE ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_ENABLED_GAD ),
							Child, HVSpace,
						End,
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_PLAYSAMPLE_INFO ),
						Child, HGroup,
							Child, objs[ GID_MODEONTABOPENING ] = MUICreateCycle( MSG_MUICLASS_SETTINGSSOUND_TABOPENING_GAD, TAB_CYCLE_TABOPENMODES, MSG_CY_NEVER, MSG_CY_WINDOWISINACTIVE ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_TABOPENING_GAD ),
							Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_TABOPENING_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, &objs[ GID_SPLONTABOPENING ], TAG_DONE ),
						End,
						Child, HGroup,
							Child, objs[ GID_MODEONHIGHLIGHT  ] = MUICreateCycle( MSG_MUICLASS_SETTINGSSOUND_ONHIGHLIGHT_GAD, TAB_CYCLE_HIGHLIGHTMODES, MSG_CY_NEVER, MSG_CY_TABISINACTIVE ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_ONHIGHLIGHT_GAD ),
							Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_ONHIGHLIGHT_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, &objs[ GID_SPLONHIGHLIGHT ], TAG_DONE ),
						End,
						Child, HGroup,
							Child, objs[ GID_MODEONPRIVMSG    ] = MUICreateCycle( MSG_MUICLASS_SETTINGSSOUND_ONPRIVMSG_GAD, TAB_CYCLE_HIGHLIGHTMODES, MSG_CY_NEVER, MSG_CY_TABISINACTIVE ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_ONPRIVMSG_GAD ),
							Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_ONPRIVMSG_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, &objs[ GID_SPLONPRIVMSG ], TAG_DONE ),
						End,
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_CTCPSAMPLES_GAD ),
						Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_CTCPSAMPLES_GAD, SAMPLENAME_SIZEOF, MUII_PopDrawer, &objs[ GID_CTCPSAMPLES], TAG_DONE ),
						Child, VSpace(5),
						Child, HGroup,
							Child, objs[ GID_USEEXTERNALPLAYER    ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSSOUND_USESAMPLEPLAYER_GAD, TRUE ),
							Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_USESAMPLEPLAYER_GAD ),
							Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_USESAMPLEPLAYER_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, &objs[ GID_EXTERNALPLAYER ], TAG_DONE ),
						End,
						Child, HVSpace,
					End,
					//Child, HVSpace,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_ENABLED          ], MUIM_Notify      , MUIA_Selected    , MUIV_EveryTime, obj, 1, MM_SETTINGSSOUND_DISENABLE );
		DoMethod( objs[ GID_MODEONTABOPENING ], MUIM_Notify      , MUIA_Cycle_Active, MUIV_EveryTime, obj, 1, MM_SETTINGSSOUND_DISENABLE );
		DoMethod( objs[ GID_MODEONHIGHLIGHT  ], MUIM_Notify      , MUIA_Cycle_Active, MUIV_EveryTime, obj, 1, MM_SETTINGSSOUND_DISENABLE );
		DoMethod( objs[ GID_MODEONPRIVMSG    ], MUIM_Notify      , MUIA_Cycle_Active, MUIV_EveryTime, obj, 1, MM_SETTINGSSOUND_DISENABLE );
		DoMethod( objs[ GID_USEEXTERNALPLAYER], MUIM_Notify      , MUIA_Selected    , MUIV_EveryTime, obj, 1, MM_SETTINGSSOUND_DISENABLE );



		DoMethod( obj, MM_SETTINGSSOUND_RESETTODEFAULTS );

		return( (IPTR) obj );
    }
	return( (IPTR) NULL );
}
/* \\\ */
/* /// MM_ContextMenuSelect()
*/

/*************************************************************************/

static IPTR MM_ContextMenuSelect( struct IClass *cl, Object *obj, struct  MUIP_ContextMenuChoice *msg )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( msg->item ) {
		switch( MUIGetVar( msg->item, MUIA_UserData ) ) {
			case GID_CMENU_RESETTODEFAULTS:
				DoMethod( obj, MM_SETTINGSSOUND_RESETTODEFAULTS );
				break;
		}
	}
	return( 0 );
}
/* \\\ */

/* /// MM_ResetToDefaults()
**
*/

/*************************************************************************/

static IPTR MM_ResetToDefaults( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr, TAB_CONFIGITEMS[ i ].Default, MUIA_ObjectID, TAB_CONFIGITEMS[ i ].ObjectID, TAG_DONE );
	}

	DoMethod( obj, MM_SETTINGSSOUND_DISENABLE );

	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static IPTR MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSSOUND_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ; TAB_CONFIGITEMS[ i ].GadgetID != -1 ; i++ ) {
		if( TAB_CONFIGITEMS[ i ].ObjectID == msg->ObjectID ) {
			return( (ULONG) MUIGetVar( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ], TAB_CONFIGITEMS[ i ].Attr ) );
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_DisEnable()
**
*/

/*************************************************************************/

static IPTR MM_DisEnable( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
BOOL global, tab, highlight, privmsg, ext;

	global    = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_ENABLED            ], MUIA_Selected      ) ? FALSE : TRUE );
	tab       = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_MODEONTABOPENING   ], MUIA_Cycle_Active  ) ? FALSE : TRUE );
	highlight = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_MODEONHIGHLIGHT    ], MUIA_Cycle_Active  ) ? FALSE : TRUE );
	privmsg   = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_MODEONPRIVMSG      ], MUIA_Cycle_Active  ) ? FALSE : TRUE );
	ext       = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_USEEXTERNALPLAYER  ], MUIA_Selected      ) ? FALSE : TRUE );

	SetAttrs( mccdata->mcc_ClassObjects[ GID_MODEONTABOPENING   ], MUIA_Disabled, global            , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SPLONTABOPENING    ], MUIA_Disabled, global | tab      , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_MODEONHIGHLIGHT    ], MUIA_Disabled, global            , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SPLONHIGHLIGHT     ], MUIA_Disabled, global | highlight, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_MODEONPRIVMSG      ], MUIA_Disabled, global            , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_SPLONPRIVMSG       ], MUIA_Disabled, global |privmsg   , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CTCPSAMPLES        ], MUIA_Disabled, global            , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_USEEXTERNALPLAYER  ], MUIA_Disabled, global            , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_EXTERNALPLAYER     ], MUIA_Disabled, global | ext      , TAG_DONE );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsSound_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsSound_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                               : return( OM_New                   ( cl, obj, (APTR) msg ) );
		case MUIM_ContextMenuChoice               : return( MM_ContextMenuSelect     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSOUND_RESETTODEFAULTS     : return( MM_ResetToDefaults       ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSOUND_READCONFIG          : return( MM_ReadConfig            ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSOUND_DISENABLE           : return( MM_DisEnable             ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsSound_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsSound_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSSOUND ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsSound_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSSOUND ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsSound_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsSound_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSSOUND ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSSOUND ] );
		appclasses[ CLASSID_SETTINGSSOUND ] = NULL;
    }
}
/* \\\ */



