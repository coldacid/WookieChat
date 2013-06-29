/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingslog.c
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
#include "muiclass_settingslog.h"
#include "version.h"

/*************************************************************************/

 
/*
** gadgets used by this class
*/

enum
{
GID_ACTIVEPUBLIC = 0,
GID_ACTIVEPRIVATE,
GID_RECOVERPUBLIC,
GID_RECOVERPRIVATE,
GID_SPLIT,
GID_SPLITLIMIT,
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
	{ GID_ACTIVEPUBLIC      , OID_LOG_ACTIVEPUBLIC     , MUIA_Selected      ,  (LONG) 1    },
	{ GID_ACTIVEPRIVATE     , OID_LOG_ACTIVEPRIVATE    , MUIA_Selected      ,  (LONG) 1    },
	{ GID_RECOVERPUBLIC     , OID_LOG_RECOVERPUBLIC    , MUIA_String_Integer,  (LONG) 40   },
	{ GID_RECOVERPRIVATE    , OID_LOG_RECOVERPRIVATE   , MUIA_String_Integer,  (LONG) 40   },
	{ GID_SPLIT             , OID_LOG_SPLIT            , MUIA_Selected      ,  (LONG) 1    },
	{ GID_SPLITLIMIT        , OID_LOG_SPLITLIMIT       , MUIA_String_Integer,  (LONG) 1024 },
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

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, TRUE,
			Child, HVSpace,
			Child, VGroup,
				Child, HVSpace,
				Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_LOGACTIVEFOR_INFO ),
				Child, HGroup,
					Child, objs[ GID_ACTIVEPUBLIC   ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSLOG_PUBLIC_GAD, FALSE ),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_PUBLIC_GAD ),
					Child, HVSpace,
				End,
				Child, HGroup,
					Child, objs[ GID_ACTIVEPRIVATE  ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSLOG_PRIVATE_GAD, FALSE ),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_PRIVATE_GAD ),
					Child, HVSpace,
				End,

				Child, VSpace(5),
				Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_LINESTORECOVERFROMLOG_INFO ),
				Child, HGroup,
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_LINESTORECOVERPUBLIC_GAD ),
					Child, objs[ GID_RECOVERPUBLIC  ] = MUICreateInteger( MSG_MUICLASS_SETTINGSLOG_LINESTORECOVERPUBLIC_GAD, 6 ),
					Child, HVSpace,
				End,
				Child, HGroup,
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_LINESTORECOVERPRIVATE_GAD ),
					Child, objs[ GID_RECOVERPRIVATE ] = MUICreateInteger( MSG_MUICLASS_SETTINGSLOG_LINESTORECOVERPRIVATE_GAD, 6 ),
					Child, HVSpace,
				End,
				Child, VSpace(5),
				Child, HGroup,
					Child, objs[ GID_SPLIT              ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSLOG_LOGSPLIT_GAD, FALSE ),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_LOGSPLIT_GAD ),
					Child, objs[ GID_SPLITLIMIT         ] = MUICreateInteger( MSG_MUICLASS_SETTINGSLOG_LOGSPLITLIMIT_GAD, 6 ),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSLOG_LOGSPLITLIMIT_GAD ),
					Child, HVSpace,
				End,
				Child, HVSpace,
			End,
			Child, HVSpace,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_ACTIVEPUBLIC  ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_RECOVERPUBLIC  ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );
		DoMethod( objs[ GID_ACTIVEPRIVATE ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_RECOVERPRIVATE ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );
		DoMethod( objs[ GID_SPLIT         ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[ GID_SPLITLIMIT     ], 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue );

		DoMethod( obj, MM_SETTINGSLOG_RESETTODEFAULTS );

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
		SetAttrs( mccdata->mcc_ClassObjects[ TAB_CONFIGITEMS[ i ].GadgetID ],
								TAB_CONFIGITEMS[ i ].Attr, TAB_CONFIGITEMS[ i ].Default,
								MUIA_ObjectID            , TAB_CONFIGITEMS[ i ].ObjectID,
								TAG_DONE );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSLOG_READCONFIG *msg )
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

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsLog_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsLog_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                              : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSLOG_RESETTODEFAULTS     : return( MM_ResetToDefaults         ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSLOG_READCONFIG          : return( MM_ReadConfig              ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsLog_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsLog_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSLOG ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsLog_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSLOG ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsLog_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsLog_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSLOG ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSLOG ] );
		appclasses[ CLASSID_SETTINGSLOG ] = NULL;
    }
}
/* \\\ */



