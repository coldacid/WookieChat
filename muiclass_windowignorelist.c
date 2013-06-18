/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowignorelist.c
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
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_ignorelist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowignorelist.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_LIST = 0,
GID_PATTERN,
GID_PRIVMSG,
GID_CTCP,
GID_DCC,
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

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWIGNORELIST_TITLE ),
			MUIA_Window_ID               , MAKE_ID('I','G','N','O'),
			MUIA_Window_NoMenus	       	 , TRUE,
			WindowContents, VGroup,
					Child, MUICreateLabelLeft( MSG_IGNORE_LONG_DETAILED_HELP ),
					Child, NListviewObject, MUIA_NListview_NList, objs[ GID_LIST ] = IgnoreListObject, End, End,
					Child, HGroup,
						Child, objs[ GID_ADD    ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWIGNORELIST_ADD_GAD   ),
						Child, objs[ GID_REMOVE ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWIGNORELIST_REMOVE_GAD ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_WINDOWIGNORELIST_PATTERN_GAD ),
						Child, objs[ GID_PATTERN ] = MUICreateStringFixed( MSG_MUICLASS_WINDOWIGNORELIST_PATTERN_GAD, IGNOREENTRY_PATTERN_SIZEOF ),
						Child, objs[ GID_PRIVMSG ] = MUICreateCheckbox( MSG_MUICLASS_WINDOWIGNORELIST_PRIVMSG_GAD, TRUE ),
						Child, MUICreateLabelLeft(  MSG_MUICLASS_WINDOWIGNORELIST_PRIVMSG_GAD ),
						Child, objs[ GID_CTCP    ] = MUICreateCheckbox( MSG_MUICLASS_WINDOWIGNORELIST_CTCP_GAD, TRUE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_WINDOWIGNORELIST_CTCP_GAD ),
						Child, objs[ GID_DCC     ] = MUICreateCheckbox( MSG_MUICLASS_WINDOWIGNORELIST_DCC_GAD, TRUE ),
						Child, MUICreateLabelLeft( MSG_MUICLASS_WINDOWIGNORELIST_DCC_GAD ),
					End,
				End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj             , 3, MUIM_Set, MUIA_Window_Open, FALSE );
		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, objs[ GID_LIST ], 2, MM_IGNORELIST_EXPORTLISTASTEXT, DEFAULT_SETTINGSIGNORE_NAME );

		DoMethod( objs[ GID_LIST       ], MUIM_Notify, MUIA_NList_Active   , MUIV_EveryTime, obj             , 1, MM_WINDOWIGNORELIST_LISTTOGADGETS );
		DoMethod( objs[ GID_PATTERN    ], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, obj             , 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );
		DoMethod( objs[ GID_PRIVMSG    ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj             , 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );
		DoMethod( objs[ GID_CTCP       ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj             , 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );
		DoMethod( objs[ GID_DCC        ], MUIM_Notify, MUIA_Selected       , MUIV_EveryTime, obj             , 1, MM_WINDOWIGNORELIST_GADGETSTOLIST );

		DoMethod( objs[ GID_ADD        ], MUIM_Notify, MUIA_Pressed        , FALSE         , objs[ GID_LIST ], 3, MM_IGNORELIST_ADD, 0, 0 );
		DoMethod( objs[ GID_REMOVE     ], MUIM_Notify, MUIA_Pressed        , FALSE         , objs[ GID_LIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );


		DoMethod( objs[ GID_LIST       ], MM_IGNORELIST_IMPORTLISTASTEXT, DEFAULT_SETTINGSIGNORE_NAME );

		DoMethod( obj, MM_WINDOWIGNORELIST_LISTTOGADGETS );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static ULONG OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWIGNORELIST ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
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
BOOL disabled = TRUE;

	GetAttr( MUIA_NList_Active, mccdata->mcc_ClassObjects[ GID_LIST ], (IPTR*) &pos );
	if( pos >= 0 ) {
		disabled = FALSE;
	}

	SetAttrs( mccdata->mcc_ClassObjects[ GID_PATTERN ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_PRIVMSG ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_CTCP    ], MUIA_Disabled, disabled, TAG_DONE );
	SetAttrs( mccdata->mcc_ClassObjects[ GID_DCC     ], MUIA_Disabled, disabled, TAG_DONE );
	
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
struct IgnoreEntry *ie = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ie );
	if( ie ) {

		STRPTR val;
		if( ( val = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_PATTERN ], MUIA_String_Contents ) ) ) {
			strcpy( (char *) ie->ie_Pattern, (char *) val );
		}

		ie->ie_Flags  = ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_PRIVMSG ], MUIA_Selected ) ) ? IGNOREENTRYF_TEXT : 0;
		ie->ie_Flags |= ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_CTCP    ], MUIA_Selected ) ) ? IGNOREENTRYF_CTCP : 0;
		ie->ie_Flags |= ( MUIGetVar( mccdata->mcc_ClassObjects[ GID_DCC     ], MUIA_Selected ) ) ? IGNOREENTRYF_DCC  : 0;

		DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_Redraw, MUIV_NList_Redraw_Active );
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
struct IgnoreEntry *ie = NULL;

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ie );
	if( ie ) {
		SetAttrs( mccdata->mcc_ClassObjects[ GID_PATTERN ], MUIA_NoNotify, TRUE, MUIA_String_Contents, ie->ie_Pattern, TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_PRIVMSG ], MUIA_NoNotify, TRUE, MUIA_Selected       , ( ( ie->ie_Flags & IGNOREENTRYF_TEXT ) ? TRUE : FALSE ), TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_CTCP    ], MUIA_NoNotify, TRUE, MUIA_Selected       , ( ( ie->ie_Flags & IGNOREENTRYF_CTCP ) ? TRUE : FALSE ), TAG_DONE );
		SetAttrs( mccdata->mcc_ClassObjects[ GID_DCC     ], MUIA_NoNotify, TRUE, MUIA_Selected       , ( ( ie->ie_Flags & IGNOREENTRYF_DCC  ) ? TRUE : FALSE ), TAG_DONE );
	}

	DoMethod( obj, MM_WINDOWIGNORELIST_DISENABLE );

	return( 0 );
}
/* \\\ */
/* /// MM_CheckIgnore()
**
*/

/*************************************************************************/

static ULONG MM_CheckIgnore( struct IClass *cl, Object *obj, struct MP_WINDOWIGNORELIST_CHECKIGNORE *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct IgnoreEntry *ie;
char pattern[ IGNOREENTRY_PATTERN_SIZEOF * 3 ];
char *nick;
ULONG i, length, result = FALSE;

	length = strlen( (char *) msg->Nick ) + strlen( (char *) msg->HostName ) + 2;

	if( ( nick = AllocVec( length, MEMF_ANY ) ) ) {

		sprintf( nick, "%s!%s", msg->Nick, msg->HostName );

		for( i = 0 ; ; i++ ) {
			ie = NULL;
			DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, i, &ie );
			if( ie ) {
				ParsePatternNoCase( (_ub_cs) ie->ie_Pattern, (STRPTR)  pattern, IGNOREENTRY_PATTERN_SIZEOF * 3 );
				if( MatchPatternNoCase( (_ub_cs) pattern, (_ub_cs) nick ) ) {
					/* match */
					switch( msg->Type ) {
						case PRIVMSG:
							if( ie->ie_Flags & IGNOREENTRYF_TEXT ) {
								result = TRUE;
							}
							break;
						case CTCP:
							if( ie->ie_Flags & IGNOREENTRYF_CTCP ) {
								result = TRUE;
							}
							break;
						case DCC:
							if( ie->ie_Flags & IGNOREENTRYF_DCC ) {
								result = TRUE;
							}
							break;
					}
					break;
				}
			} else {
				break;
			}
		}
		FreeVec( nick );
	}
	return( result );
}
/* \\\ */
 
/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowIgnoreList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowIgnoreList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                                : return( OM_New           ( cl, obj, (APTR) msg ) );
		case OM_GET                                : return( OM_Get           ( cl, obj, (APTR) msg ) );

		case MM_WINDOWIGNORELIST_DISENABLE         : return( MM_DisEnable     ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_GADGETSTOLIST     : return( MM_GadgetsToList ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_LISTTOGADGETS     : return( MM_ListToGadgets ( cl, obj, (APTR) msg ) );
		case MM_WINDOWIGNORELIST_CHECKIGNORE       : return( MM_CheckIgnore   ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowIgnoreList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowIgnoreList_InitClass( void )
{
	appclasses[ CLASSID_WINDOWIGNORELIST ] = MUI_CreateCustomClass( NULL, (ClassID)MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowIgnoreList_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWIGNORELIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowIgnoreList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowIgnoreList_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWIGNORELIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWIGNORELIST ] );
		appclasses[ CLASSID_WINDOWIGNORELIST ] = NULL;
    }
}
/* \\\ */

