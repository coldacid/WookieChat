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
#include "muiclass_aliaslist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_settingsalias.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_ALIASLIST = 0,
GID_ALIAS,
GID_TEXT,
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

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *) DoSuperNew( cl, obj, MUIA_Group_Horiz, FALSE,
				Child, NListviewObject, MUIA_NListview_NList, objs[ GID_ALIASLIST ] = AliasListObject, End, End,
				Child, HGroup,
					Child, objs[ GID_ADD       ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSALIAS_ADD_GAD ),
					Child, objs[ GID_REMOVE    ] = MUICreateSmallButton( MSG_MUICLASS_SETTINGSALIAS_REMOVE_GAD ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSALIAS_ALIAS_GAD ),
					Child, objs[ GID_ALIAS     ] = MUICreateStringFixed( MSG_MUICLASS_SETTINGSALIAS_ALIAS_GAD, ALIASENTRY_ALIAS_SIZEOF ),
					Child, MUICreateLabel( MSG_MUICLASS_SETTINGSALIAS_TEXT_GAD ),
					Child, objs[ GID_TEXT      ] = MUICreateString( MSG_MUICLASS_SETTINGSALIAS_TEXT_GAD, ALIASENTRY_TEXT_SIZEOF ),
				End,

		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));
		
		SetAttrs( objs[ GID_ALIAS     ], MUIA_String_Reject, " ", TAG_DONE );

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_ADD       ], MUIM_Notify, MUIA_Pressed, FALSE, objs[ GID_ALIASLIST ], 3, MM_ALIASLIST_ADD, 0, 0 );
		DoMethod( objs[ GID_REMOVE    ], MUIM_Notify, MUIA_Pressed, FALSE, objs[ GID_ALIASLIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );

		DoMethod( objs[ GID_ALIASLIST ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj, 1, MM_SETTINGSALIAS_LISTTOGADGETS );
		DoMethod( objs[ GID_ALIAS     ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSALIAS_GADGETSTOLIST );
		DoMethod( objs[ GID_TEXT      ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj, 1, MM_SETTINGSALIAS_GADGETSTOLIST );

		DoMethod( obj, MM_SETTINGSALIAS_LISTTOGADGETS );

		return( (IPTR) obj );
    }
	return( (IPTR) NULL );
}
/* \\\ */

/* /// MM_DisEnable()
**
*/

/*************************************************************************/

static IPTR MM_DisEnable( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG pos;
BOOL disabled = TRUE;

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	pos = MUIGetVar( mccdata->mcc_ClassObjects[ GID_ALIASLIST ], MUIA_NList_Active );
	if( pos >= 0 ) {
		disabled = FALSE;
	}

	SetAttrs( mccdata->mcc_ClassObjects[ GID_REMOVE  ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_ALIAS   ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_TEXT    ], MUIA_Disabled, disabled, TAG_DONE );

	return( 0 );
}
/* \\\ */
/* /// MM_GadgetsToList()
**
*/

/*************************************************************************/

static IPTR MM_GadgetsToList( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct AliasEntry *ae = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_ALIASLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ae );

	if( ae ) {
		STRPTR str;
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_ALIAS ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ae->ae_Alias, (char *) str );
		}
		if( ( str = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_TEXT ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ae->ae_Text, (char *) str );
		}
		DoMethod( mccdata->mcc_ClassObjects[ GID_ALIASLIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ListToGadgets()
**
*/

/*************************************************************************/

static IPTR MM_ListToGadgets( struct IClass *cl, Object *obj, Msg *msg UNUSED )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct AliasEntry *ae = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_ALIASLIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ae );

	if( ae ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_ALIAS ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ae->ae_Alias, TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_TEXT  ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ae->ae_Text , TAG_DONE );
	}

	DoMethod( obj, MM_SETTINGSALIAS_DISENABLE );

	return( 0 );
}
/* \\\ */
/* /// MM_ReadConfig()
**
*/

/*************************************************************************/

static IPTR MM_ReadConfig( struct IClass *cl, Object *obj, struct MP_SETTINGSALIAS_READCONFIG *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	switch( msg->ObjectID ) {
		case OID_ALI_LIST:
			return( (IPTR) mccdata->mcc_ClassObjects[ GID_ALIASLIST ] );
		default:
			return( 0 );
	}
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_SettingsAlias_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_SettingsAlias_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSALIAS_DISENABLE         : return( MM_DisEnable                     ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSALIAS_GADGETSTOLIST     : return( MM_GadgetsToList                 ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSALIAS_LISTTOGADGETS     : return( MM_ListToGadgets                 ( cl, obj, (APTR) msg ) );
		case MM_SETTINGSALIAS_READCONFIG        : return( MM_ReadConfig  ( cl, obj, (APTR) msg ) );

	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_SettingsAlias_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_SettingsAlias_InitClass( void )
{
	appclasses[ CLASSID_SETTINGSALIAS ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Group, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_SettingsAlias_Dispatcher) );
	return( appclasses[ CLASSID_SETTINGSALIAS ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_SettingsAlias_DisposeClass()
**
*/

/*************************************************************************/

void MCC_SettingsAlias_DisposeClass( void )
{
	if( appclasses[ CLASSID_SETTINGSALIAS ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SETTINGSALIAS ] );
		appclasses[ CLASSID_SETTINGSALIAS ] = NULL;
    }
}
/* \\\ */



