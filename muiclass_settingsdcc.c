/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingsdcc.c
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

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsdcc.h"
#include "version.h"

/*************************************************************************/

#define DCCADDRESS_SIZEOF 0x200
#define DCCPATH_SIZEOF 0x200

/*
** gadgets used by this class
*/

enum
{
GID_USELOCALADDRESS,
GID_USECUSTOMADDRESS,
GID_CUSTOMADDRESS,
GID_RANGEFROM,
GID_RANGETO,
GID_INPATH,
GID_INACCEPT,
GID_INWRITEMODE,
GID_OUTPATH,
GID_LAST,
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
	{ GID_USELOCALADDRESS    , OID_DCC_USELOCALADDRESS , MUIA_Selected       , (ULONG) 1      },
	{ GID_USECUSTOMADDRESS   , OID_DCC_USECUSTOMADDRESS, MUIA_Selected       , (ULONG) 1      },
	{ GID_CUSTOMADDRESS      , OID_DCC_CUSTOMADDRESS   , MUIA_String_Contents, (ULONG) ""     },
	{ GID_RANGEFROM          , OID_DCC_RANGEFROM       , MUIA_String_Integer , (ULONG) 1028   },
	{ GID_RANGETO            , OID_DCC_RANGETO         , MUIA_String_Integer , (ULONG) 1035   },
	{ GID_INPATH             , OID_DCC_INPATH          , MUIA_String_Contents, (ULONG) "RAM:" },
	{ GID_INACCEPT           , OID_DCC_INACCEPT        , MUIA_Selected       , (ULONG) 1      },
	{ GID_INWRITEMODE        , OID_DCC_INWRITEMODE     , MUIA_Cycle_Active   , (ULONG) 1      },
	{ GID_OUTPATH            , OID_DCC_OUTPATH         , MUIA_String_Contents, (ULONG) "RAM:" },
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
static STRPTR TAB_CYCLE_INWRITEMODE[ MSG_CY_CANCEL - MSG_CY_OVERWRITE + 2 ];

	debug( "%s (%ld) %s - Class: 0x08lx Object: 0x08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, TRUE,
				//Child, HVSpace,
				Child, VGroup,
					Child, HVSpace,
					Child, MUI_MakeObject( MUIO_BarTitle, LGS( MSG_MUICLASS_SETTINGSDCC_GENERAL_INFO )),
					Child, HGroup,
						Child, objs[ GID_USELOCALADDRESS ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSDCC_DCCLOCALADDRESS_GAD, FALSE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSDCC_DCCLOCALADDRESS_GAD ),
						Child, HVSpace,
					End,
					Child, HGroup,
						Child, objs[ GID_USECUSTOMADDRESS ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSDCC_DCCENTERADDRESS_GAD, FALSE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSDCC_DCCENTERADDRESS_GAD ),
						Child, objs[ GID_CUSTOMADDRESS    ] = MUICreateString( MSG_MUICLASS_SETTINGSDCC_DCCADDRESS_GAD, DCCADDRESS_SIZEOF ),
						Child, HVSpace,
					End,
					Child, HGroup,
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSDCC_DCCPORTRANGEFROM_GAD ),
						Child, objs[ GID_RANGEFROM    ] = MUICreateInteger( MSG_MUICLASS_SETTINGSDCC_DCCPORTRANGEFROM_GAD, 6 ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSDCC_DCCPORTRANGETO_GAD ),
						Child, objs[ GID_RANGETO      ] = MUICreateInteger( MSG_MUICLASS_SETTINGSDCC_DCCPORTRANGETO_GAD, 6 ),
						Child, HVSpace,
					End,
					Child, VSpace(5),
					Child, MUI_MakeObject( MUIO_BarTitle, LGS( MSG_MUICLASS_SETTINGSDCC_INCOMING_INFO )),
					Child, HGroup,
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSDCC_DCCINPATH_GAD ),
						Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSDCC_DCCINPATH_GAD, DCCPATH_SIZEOF, MUII_PopFile, &objs[ GID_INPATH ], TAG_DONE ),
					End,
					Child, HGroup,
						Child, objs[ GID_INACCEPT ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSDCC_DCCINACCEPT_GAD, FALSE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSDCC_DCCINACCEPT_GAD ),
						Child, HVSpace,
					End,
					Child, HGroup,
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSDCC_DCCINWRITEMODE_GAD ),
						Child, objs[ GID_INWRITEMODE ] = MUICreateCycle( MSG_MUICLASS_SETTINGSDCC_DCCINWRITEMODE_GAD, &TAB_CYCLE_INWRITEMODE, MSG_CY_OVERWRITE, MSG_CY_CANCEL ),
					End,
					Child, VSpace(5),
					Child, MUI_MakeObject( MUIO_BarTitle, LGS( MSG_MUICLASS_SETTINGSDCC_OUTGOING_INFO )),
					Child, HGroup,
						Child, MUICreateLabel( MSG_MUICLASS_SETTINGSDCC_DCCOUTPATH_GAD ),
						Child, MUICreatePopASL( MSG_MUICLASS_SETTINGSDCC_DCCOUTPATH_GAD, DCCPATH_SIZEOF, MUII_PopFile, &objs[ GID_OUTPATH ], TAG_DONE ),
					End,
					Child, HVSpace,
				End,
				//Child, HVSpace,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_USELOCALADDRESS  ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_USECUSTOMADDRESS ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );
		DoMethod( objs[ GID_USELOCALADDRESS  ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_CUSTOMADDRESS    ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );
		DoMethod( objs[ GID_USECUSTOMADDRESS ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_CUSTOMADDRESS    ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );

		DoMethod( obj, MM_SETTINGSDCC_RESETTODEFAULTS );

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

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSDCC_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;

	debug( "%s (%ld) %s - Class: 0x08lx Object: 0x08lx \n", __FILE__, __LINE__, __func__, cl, obj );

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

/* /// MCC_SettingsDCC_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsDCC_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                              : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSDCC_RESETTODEFAULTS     : return( MM_ResetToDefaults         ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSDCC_READCONFIG          : return( MM_ReadConfig              ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsDCC_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsDCC_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSDCC ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsDCC_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSDCC ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsDCC_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsDCC_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSDCC ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSDCC ] );
		appclasses[ CLASSID_SETTINGSDCC ] = NULL;
    }
}
/* \\\ */



