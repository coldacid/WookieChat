/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_settingsalias.c
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
#include "muiclass_eventlist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsevent.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/


/*
** gadgets used by this class
*/

enum
{
GID_EVENTLIST = 0,
GID_MODE,
GID_POPSCRIPT,
GID_SCRIPT,
GID_TEXT,
GID_LAST
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
static STRPTR TAB_CYCLE_EVENTMODES[ MSG_MUICLASS_EVENTLIST_WINDOWISINACTIVE - MSG_MUICLASS_EVENTLIST_MODENEVER + 2 ];

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, FALSE,
				Child, TextObject, MUIA_Text_PreParse, NULL,
									MUIA_Text_Contents, LGS( MSG_MUICLASS_SETTINGSEVENT_INFO ),
									MUIA_InnerLeft, 0,
									MUIA_InnerRight, 0,
									End,

				Child, NListviewObject, MUIA_NListview_NList, objs[ GID_EVENTLIST ] = EventListObject, End, End,
				Child, HGroup,
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSEVENT_MODE_GAD ),
					Child, objs[ GID_MODE      ] = MUICreateCycle( MSG_MUICLASS_SETTINGSEVENT_MODE_GAD, TAB_CYCLE_EVENTMODES, MSG_MUICLASS_EVENTLIST_MODENEVER, MSG_MUICLASS_EVENTLIST_WINDOWISINACTIVE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSEVENT_SCRIPT_GAD ),
					Child, objs[ GID_POPSCRIPT ] = MUICreatePopASL( MSG_MUICLASS_SETTINGSEVENT_SCRIPT_GAD, EVENTENTRY_SCRIPT_SIZEOF, MUII_PopFile, &objs[ GID_SCRIPT ], TAG_DONE ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSEVENT_ARGUMENT_GAD ),
					Child, objs[ GID_TEXT      ] = MUICreateString( MSG_MUICLASS_SETTINGSEVENT_ARGUMENT_GAD, EVENTENTRY_TEXT_SIZEOF ),
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));
		
		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_EVENTLIST   ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSEVENT_LISTTOGADGETS );
		DoMethod( objs[ GID_MODE        ], MUIM_Notify, MUIA_Cycle_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSEVENT_GADGETSTOLIST );
		DoMethod( objs[ GID_SCRIPT      ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSEVENT_GADGETSTOLIST );
		DoMethod( objs[ GID_TEXT        ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSEVENT_GADGETSTOLIST );

		DoMethod( obj, MM_SETTINGSEVENT_LISTTOGADGETS );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// MM_DisEnable()
**
*/

/*************************************************************************/

static ULONG MM_DisEnable( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG pos;
BOOL events = TRUE;

	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_EVENTLIST ], MUIA_NList_Active );
	if( pos >= 0 ) {
		events = FALSE;
	}

	SetAttrs( mccdata->mcc_ClassObjects[ GID_MODE       ], MUIA_Disabled, events, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_POPSCRIPT  ], MUIA_Disabled, events, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_TEXT       ], MUIA_Disabled, events, TAG_DONE );

	return( 0 );
}
/* \\\ */
/* /// MM_GadgetsToList()
**
*/

/*************************************************************************/

static ULONG MM_GadgetsToList( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct EventEntry *ee = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_EVENTLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ee );

	if( ee ) {
		STRPTR str;
		ee->ee_Mode = MUIGetVar( mccdata->mcc_ClassObjects[ GID_MODE ], MUIA_Cycle_Active );
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_SCRIPT  ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ee->ee_Script , (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_TEXT    ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ee->ee_Text   , (char *) str );
		}
		DoMethod( mccdata->mcc_ClassObjects[ GID_EVENTLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ListToGadgets()
**
*/

/*************************************************************************/

static ULONG MM_ListToGadgets( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct EventEntry *ee = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_EVENTLIST    ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ee );

	if( ee ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_MODE    ], MUIA_NoNotify, TRUE, MUIA_Cycle_Active   , ee->ee_Mode  , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_SCRIPT  ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ee->ee_Script, TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_TEXT    ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ee->ee_Text  , TAG_DONE );
	}

	DoMethod( obj, MM_SETTINGSEVENT_DISENABLE );

	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSEVENT_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	switch( msg->ObjectID ) {
		case OID_EVT_LIST:
			return( (ULONG) mccdata->mcc_ClassObjects[ GID_EVENTLIST ] );
		default:
			return( 0 );
	}
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsEvent_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsEvent_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                              : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSEVENT_DISENABLE         : return( MM_DisEnable                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSEVENT_GADGETSTOLIST     : return( MM_GadgetsToList                 ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSEVENT_LISTTOGADGETS     : return( MM_ListToGadgets                 ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSEVENT_READCONFIG        : return( MM_ReadConfig  ( cl, obj, (APTR) msg ) );

	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsEvent_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsEvent_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSEVENT ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsEvent_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSEVENT ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsEvent_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsEvent_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSEVENT ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSEVENT ] );
		appclasses[ CLASSID_SETTINGSEVENT ] = NULL;
    }
}
/* \\\ */


