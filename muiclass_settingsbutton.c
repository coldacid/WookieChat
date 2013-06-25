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

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_buttonlist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsbutton.h"
#include "version.h"

/*************************************************************************/


/*
** gadgets used by this class
*/

enum
{
GID_SHOWBUTTONS = 0,
GID_BUTTONLIST,
GID_NAME,
GID_COMMAND,
GID_ADD,
GID_REMOVE,
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

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, FALSE,
				Child, HGroup,
					Child, objs[ GID_SHOWBUTTONS   ] = MUICreateCheckbox( MSG_MUICLASS_SETTINGSBUTTON_SHOWBUTTONS_GAD, TRUE ),
					Child, MUICreateLabelLeft( MSG_MUICLASS_SETTINGSBUTTON_SHOWBUTTONS_GAD ),
					Child, HVSpace,
				End,
				Child, TextObject, MUIA_Text_PreParse, NULL,
									MUIA_Text_Contents, LGS( MSG_MUICLASS_SETTINGSBUTTON_INFO ),
									MUIA_InnerLeft, 0,
									MUIA_InnerRight, 0,
									End,

				Child, NListviewObject, MUIA_NListview_NList, objs[ GID_BUTTONLIST ] = ButtonListObject, End, End,
				Child, HGroup,
					Child, objs[ GID_ADD       ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSBUTTON_ADD_GAD ),
					Child, objs[ GID_REMOVE    ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSBUTTON_REMOVE_GAD ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSBUTTON_NAME_GAD ),
					Child, objs[ GID_NAME      ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSBUTTON_COMMAND_GAD, BUTTONENTRY_NAME_SIZEOF ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSBUTTON_COMMAND_GAD ),
					Child, objs[ GID_COMMAND   ] = MUICreateString( MSG_MUICLASS_SETTINGSBUTTON_COMMAND_GAD, BUTTONENTRY_COMMAND_SIZEOF ),
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));
		
		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_ADD         ], MUIM_Notify, MUIA_Pressed, FALSE, objs[ GID_BUTTONLIST ], 3, MM_BUTTONLIST_ADD, 0, 0 );
		DoMethod( objs[ GID_REMOVE      ], MUIM_Notify, MUIA_Pressed, FALSE, objs[ GID_BUTTONLIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );

		DoMethod( objs[ GID_BUTTONLIST  ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSBUTTON_LISTTOGADGETS );
		DoMethod( objs[ GID_NAME        ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSBUTTON_GADGETSTOLIST );
		DoMethod( objs[ GID_COMMAND     ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSBUTTON_GADGETSTOLIST );

		DoMethod( objs[ GID_SHOWBUTTONS ], MUIM_Notify, MUIA_Selected, MUIV_EveryTime, obj, 1, MM_SETTINGSBUTTON_DISENABLE );
		SetAttrs( objs[ GID_SHOWBUTTONS ], MUIA_ObjectID, OID_BUT_SHOWBUTTONS, TAG_DONE );

		DoMethod( obj, MM_SETTINGSBUTTON_LISTTOGADGETS );

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
BOOL buttons = TRUE;
BOOL list    = TRUE;

	if( MUIGetVar( mccdata->mcc_ClassObjects[ GID_SHOWBUTTONS ], MUIA_Selected ) ) {
		list = FALSE;
		pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_BUTTONLIST ], MUIA_NList_Active );
		if( pos >= 0 ) {
			buttons = FALSE;
		}
	}
	SetAttrs( mccdata->mcc_ClassObjects[ GID_BUTTONLIST ], MUIA_Disabled, list   , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_ADD        ], MUIA_Disabled, list   , TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_REMOVE     ], MUIA_Disabled, buttons, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_NAME       ], MUIA_Disabled, buttons, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_COMMAND    ], MUIA_Disabled, buttons, TAG_DONE );

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
struct ButtonEntry *be = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_BUTTONLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &be );

	if( be ) {
		STRPTR str;
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_NAME    ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) be->be_Name   , (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_COMMAND ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) be->be_Command, (char *) str );
		}
		DoMethod( mccdata->mcc_ClassObjects[ GID_BUTTONLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
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
struct ButtonEntry *be = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_BUTTONLIST    ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &be );

	if( be ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_NAME    ], MUIA_NoNotify, TRUE, MUIA_String_Contents, be->be_Name   , TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_COMMAND ], MUIA_NoNotify, TRUE, MUIA_String_Contents, be->be_Command, TAG_DONE );
	}

	DoMethod( obj, MM_SETTINGSBUTTON_DISENABLE );

	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static ULONG MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSBUTTON_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	switch( msg->ObjectID ) {
		case OID_BUT_SHOWBUTTONS:
			return( (ULONG) MUIGetVar( mccdata->mcc_ClassObjects[ GID_SHOWBUTTONS ], MUIA_Selected ) );
		case OID_BUT_LIST:
			return( (ULONG) mccdata->mcc_ClassObjects[ GID_BUTTONLIST ] );
		default:
			return( 0 );
	}
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsButton_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsButton_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                              : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSBUTTON_DISENABLE         : return( MM_DisEnable                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSBUTTON_GADGETSTOLIST     : return( MM_GadgetsToList                 ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSBUTTON_LISTTOGADGETS     : return( MM_ListToGadgets                 ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSBUTTON_READCONFIG        : return( MM_ReadConfig  ( cl, obj, (APTR) msg ) );

	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsButton_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsButton_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSBUTTON ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ),  (APTR) ENTRY(MCC_SettingsButton_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSBUTTON ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsButton_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsButton_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSBUTTON ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSBUTTON ] );
		appclasses[ CLASSID_SETTINGSBUTTON ] = NULL;
    }
}
/* \\\ */


