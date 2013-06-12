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
#include "muiclass_settingssound.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

#define SAMPLENAME_SIZEOF 0x200
 
/*
** gadgets used by this class
*/

enum
{
GID_MODEONTABOPENING,
GID_SPLONTABOPENING,
GID_MODEONHIGHLIGHT,
GID_SPLONHIGHLIGHT,
GID_MODEONPRIVMSG,
GID_SPLONPRIVMSG,
GID_CTCPSAMPLES,
GID_USEEXTERNALPLAYER,
GID_EXTERNALPLAYER,
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
	{ GID_MODEONTABOPENING  , OID_SND_MODEONTABOPENING , MUIA_Cycle_Active   , (LONG) 1 },
	{ GID_SPLONTABOPENING   , OID_SND_SPLONTABOPENING  , MUIA_String_Contents, (LONG) "PROGDIR:Sound/Eagh" },
	{ GID_MODEONHIGHLIGHT   , OID_SND_MODEONHIGHLIGHT  , MUIA_Cycle_Active   , (LONG) 1 },
	{ GID_SPLONHIGHLIGHT    , OID_SND_SPLONHIGHLIGHT   , MUIA_String_Contents, (LONG) "PROGDIR:Sound/OhNo" },
	{ GID_MODEONPRIVMSG     , OID_SND_MODEONPRIVMSG    , MUIA_Cycle_Active   , (LONG) 1 },
	{ GID_SPLONPRIVMSG      , OID_SND_SPLONPRIVMSG     , MUIA_String_Contents, (LONG) "PROGDIR:Sound/Beep" },
	{ GID_CTCPSAMPLES       , OID_SND_CTCPSAMPLES      , MUIA_String_Contents, (LONG) "PROGDIR:Sound/" },
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

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];
static STRPTR TAB_CYCLE_TABOPENMODES[ MSG_CY_WINDOWISINACTIVE - MSG_CY_NEVER + 2 ];
static STRPTR TAB_CYCLE_HIGHLIGHTMODES[ MSG_CY_TABISINACTIVE - MSG_CY_NEVER + 2 ];

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, FALSE,

					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_PLAYSAMPLE_INFO ),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_TABOPENING_GAD ),
					Child, HGroup,
						Child, objs[ GID_MODEONTABOPENING ] = MUICreateCycle( MSG_MUICLASS_SETTINGSSOUND_TABOPENING_GAD, &TAB_CYCLE_TABOPENMODES, MSG_CY_NEVER, MSG_CY_WINDOWISINACTIVE ),
						Child, objs[ GID_SPLONTABOPENING  ] = MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_TABOPENING_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, TAG_DONE ),
					End,
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_ONHIGHLIGHT_GAD ),
					Child, HGroup,
						Child, objs[ GID_MODEONHIGHLIGHT  ] = MUICreateCycle( MSG_MUICLASS_SETTINGSSOUND_ONHIGHLIGHT_GAD, &TAB_CYCLE_HIGHLIGHTMODES, MSG_CY_NEVER, MSG_CY_TABISINACTIVE ),
						Child, objs[ GID_SPLONHIGHLIGHT   ] = MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_ONHIGHLIGHT_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, TAG_DONE ),
					End,
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_ONPRIVMSG_GAD ),
					Child, HGroup,
						Child, objs[ GID_MODEONPRIVMSG    ] = MUICreateCycle( MSG_MUICLASS_SETTINGSSOUND_ONPRIVMSG_GAD, &TAB_CYCLE_HIGHLIGHTMODES, MSG_CY_NEVER, MSG_CY_TABISINACTIVE ),
						Child, objs[ GID_SPLONPRIVMSG     ] = MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_ONPRIVMSG_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, TAG_DONE ),
					End,
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_CTCPSAMPLES_GAD ),
					Child, objs[ GID_CTCPSAMPLES          ] = MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_CTCPSAMPLES_GAD, SAMPLENAME_SIZEOF, MUII_PopDrawer, TAG_DONE ),
					Child, VSpace(5),
					Child, HGroup,
						Child, objs[ GID_USEEXTERNALPLAYER    ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSSOUND_USESAMPLEPLAYER_GAD, FALSE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSSOUND_USESAMPLEPLAYER_GAD ),
						Child, objs[ GID_EXTERNALPLAYER       ] = MUICreatePopASL( MSG_MUICLASS_SETTINGSSOUND_USESAMPLEPLAYER_GAD, SAMPLENAME_SIZEOF, MUII_PopFile, TAG_DONE ),
					End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_MODEONTABOPENING ], MUIM_Notify, MUIA_Cycle_Active, 0, objs[ GID_SPLONTABOPENING  ], 3, MUIM_Set, MUIA_Disabled, TRUE );
		DoMethod( objs[ GID_MODEONTABOPENING ], MUIM_Notify, MUIA_Cycle_Active, 1, objs[ GID_SPLONTABOPENING  ], 3, MUIM_Set, MUIA_Disabled, FALSE );
		DoMethod( objs[ GID_MODEONTABOPENING ], MUIM_Notify, MUIA_Cycle_Active, 2, objs[ GID_SPLONTABOPENING  ], 3, MUIM_Set, MUIA_Disabled, FALSE );

		DoMethod( objs[ GID_MODEONHIGHLIGHT ], MUIM_Notify, MUIA_Cycle_Active, 0, objs[ GID_SPLONHIGHLIGHT  ], 3, MUIM_Set, MUIA_Disabled, TRUE );
		DoMethod( objs[ GID_MODEONHIGHLIGHT ], MUIM_Notify, MUIA_Cycle_Active, 1, objs[ GID_SPLONHIGHLIGHT  ], 3, MUIM_Set, MUIA_Disabled, FALSE );
		DoMethod( objs[ GID_MODEONHIGHLIGHT ], MUIM_Notify, MUIA_Cycle_Active, 2, objs[ GID_SPLONHIGHLIGHT  ], 3, MUIM_Set, MUIA_Disabled, FALSE );
		DoMethod( objs[ GID_MODEONHIGHLIGHT ], MUIM_Notify, MUIA_Cycle_Active, 3, objs[ GID_SPLONHIGHLIGHT  ], 3, MUIM_Set, MUIA_Disabled, FALSE );

		DoMethod( objs[ GID_MODEONPRIVMSG ], MUIM_Notify, MUIA_Cycle_Active, 0, objs[ GID_SPLONPRIVMSG  ], 3, MUIM_Set, MUIA_Disabled, TRUE );
		DoMethod( objs[ GID_MODEONPRIVMSG ], MUIM_Notify, MUIA_Cycle_Active, 1, objs[ GID_SPLONPRIVMSG  ], 3, MUIM_Set, MUIA_Disabled, FALSE );
		DoMethod( objs[ GID_MODEONPRIVMSG ], MUIM_Notify, MUIA_Cycle_Active, 2, objs[ GID_SPLONPRIVMSG  ], 3, MUIM_Set, MUIA_Disabled, FALSE );
		DoMethod( objs[ GID_MODEONPRIVMSG ], MUIM_Notify, MUIA_Cycle_Active, 3, objs[ GID_SPLONPRIVMSG  ], 3, MUIM_Set, MUIA_Disabled, FALSE );

		
		DoMethod( obj, MM_SETTINGSSOUND_RESETTODEFAULTS );

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

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSSOUND_READCONFIG *msg )
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

/* /// MCC_SettingsSound_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsSound_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                               : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSOUND_RESETTODEFAULTS     : return( MM_ResetToDefaults         ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSSOUND_READCONFIG          : return( MM_ReadConfig              ( cl, obj, (APTR) msg ) );
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
	appclasses[ CLASSID_SETTINGSSOUND ] = MUI_CreateCustomClass( NULL, MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsSound_Dispatcher) );
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



